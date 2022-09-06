#include "DCNSGAII_DE.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../utility/nondominated_sorting/fast_sort.h"
#include "../../../record/rcr_vec_real.h"
#include <numeric>

using namespace std::placeholders;

namespace ofec {
	DCNSGAII_DE_pop::DCNSGAII_DE_pop(size_t size_pop, int id_pro) : Population<DCMOEA_ind<IndDE>>(size_pop, id_pro, GET_CONOP(id_pro).numVariables()), DCMOEA<DCMOEA_ind<IndDE>>(), m_rand_seq(size_pop) {
		for (auto& i : m_inds) {
			i->resize_vio_obj(1);
			m_offspring.emplace_back(new IndType(*i));
			m_temp_pop.emplace_back(new IndType(*i));
		}
		std::iota(m_rand_seq.begin(), m_rand_seq.end(), 0);
		m_F = 0.5;
		m_CR = 0.6;
		m_recombine_strategy = RecombineDE::Binomial;
		/*	CONTINUOUS_CAST->set_eval_monitor_flag(true);*/
	}

	void DCNSGAII_DE_pop::initialize(int id_pro, int id_alg, int id_rnd) {
		Population::initialize(id_pro, id_rnd);
		DCMOEA::initialize(GET_CONOP(id_pro).numConstraints());
		evaluate(id_pro, id_alg);
		calculate_initial_max_violation(m_inds);
		m_e = m_max_G;
		calculate_violation_objective(m_inds);
		mark_individual_efeasible(m_inds);
		Real dimension = GET_CONOP(id_pro).numVariables();
		m_R = 0.5 * pow(2 * dimension / (2 * m_inds.size() * OFEC_PI), 1.0 / dimension);



	}

	void DCNSGAII_DE_pop::sort(int id_pro) {
		std::vector<int> ranks;
		std::function<OFEC::Dominance(IndType* const&, IndType* const&)> comp = std::bind(&DCNSGAII_DE_pop::Pareto_compare, this, _1, _2, id_pro);
		std::vector<IndType*> pop;
		for (auto& i : m_inds)
			pop.emplace_back(i.get());
		NS::fast_sort<IndType*>(pop, ranks, comp);
		/*for (size_t i = 0; i < pop.size(); i++) {
			if (ranks[i] == 0) {
				std::cout << "pop[" << i << "]: ";
				for (size_t j = 0; j < CONTINUOUS_CAST->objective_size(); ++j)
					std::cout << pop[i]->objective(j) << "\t\t";
				std::cout << pop[i]->get_vio_obj()[0] << "\t\t" << pop[i]->get_vio_obj()[1] <<std::endl;
			}
		}*/
	}

	int DCNSGAII_DE_pop::evolve(int id_pro, int id_alg, int id_rnd)
	{
		int tag(0);
		if (judge_population_efeasible(m_inds)) {
			m_k++;
			if (m_k > m_max_K + 1) {
				//return Terminate;
				//m_flag = true;
			}
			else {
				reduce_boundary();
				reduce_radius(id_pro);
				mark_individual_efeasible(m_inds);
			}
		}

		//generate offspring pop
		for (size_t i = 0; i < m_inds.size(); i++) {
			std::vector<int> ridx;
			GET_RND(id_rnd).uniform.shuffle(m_rand_seq.begin(), m_rand_seq.end());
			for (int idx : m_rand_seq) {
				if (idx != i)					ridx.emplace_back(idx);
				if (ridx.size() == 3)			break;
			}
			m_inds[i]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro);
			m_inds[i]->recombine(m_CR, m_recombine_strategy, id_rnd, id_pro);
			m_offspring[i]->solut() = m_inds[i]->trial();
		}

		//evaluate offspring pop
		for (auto& i : m_offspring)
			i->evaluate(id_pro, id_alg);

		calculate_violation_objective(m_offspring);
		mark_individual_efeasible(m_offspring);

		std::vector<IndType*> pop;
		for (auto& i : m_inds)
			pop.emplace_back(i.get());
		for (auto& i : m_offspring)
			pop.emplace_back(i.get());
		caculate_nichecount(pop, id_pro);
		//select next pop
		select_next_parent_population(pop, id_pro);

		if (tag == 0 )
			m_iter++;

		return tag;
	}

	void DCNSGAII_DE_pop::select_next_parent_population(std::vector<IndType*>& pop, int id_pro)
	{
		/* Nondominated sorting based on e-Pareto domination  */
		std::vector<int> ranks;
		std::function<OFEC::Dominance(IndType* const&, IndType* const&)> comp = std::bind(&DCNSGAII_DE_pop::e_Pareto_compare, this, _1, _2,id_pro);
		NS::fast_sort<IndType*>(pop, ranks, comp);
		for (size_t i = 0; i < pop.size(); i++) {
			pop[i]->setRank(ranks[i]);
		}

		size_t cur_rank = 0;
		size_t id_ind = 0;
		while (true) {
			int count = 0;
			for (size_t i = 0; i < pop.size(); i++)
				if (pop[i]->rank() == cur_rank)
					count++;
			int size2 = id_ind + count;
			if (size2 > this->m_inds.size()) {
				break;
			}
			for (size_t i = 0; i < pop.size(); i++)
				if (pop[i]->rank() == cur_rank) {
					*m_temp_pop[id_ind] = *pop[i];
					++id_ind;
				}
			cur_rank++;
			if (id_ind >= this->m_inds.size()) break;
		}
		if (id_ind < pop.size()) {
			std::vector<int> list;	// save the individuals in the overflowed front
			for (size_t i = 0; i < pop.size(); i++)
				if (pop[i]->rank() == cur_rank)
					list.push_back(i);
			int s2 = list.size();
			std::vector<Real> density(s2);
			std::vector<Real> obj(s2);
			std::vector<int> idx(s2);
			std::vector<int> idd(s2);
			for (size_t i = 0; i < s2; i++) {
				idx[i] = i;
				density[i] = 0;
			}
			for (size_t j = 0; j < 2; j++) {
				for (size_t i = 0; i < s2; i++) {
					idd[i] = i;
					obj[i] = pop[list[i]]->get_vio_obj()[0];
				}
				mergeSort(obj, s2, idd, true, 0, s2 - 1, s2);
				density[idd[0]] += -1.0e+30;
				density[idd[s2 - 1]] += -1.0e+30;
				for (int k = 1; k < s2 - 1; k++)
					density[idd[k]] += -(obj[idd[k]] - obj[idd[k - 1]] + obj[idd[k + 1]] - obj[idd[k]]);
			}
			for (size_t j = 0; j < GET_CONOP(id_pro).numObjectives(); j++) {
				for (size_t i = 0; i < s2; i++) {
					idd[i] = i;
					obj[i] = pop[list[i]]->objective()[j];
				}
				mergeSort(obj, s2, idd, true, 0, s2 - 1, s2);
				density[idd[0]] += -1.0e+30;
				density[idd[s2 - 1]] += -1.0e+30;
				for (int k = 1; k < s2 - 1; k++)
					density[idd[k]] += -(obj[idd[k]] - obj[idd[k - 1]] + obj[idd[k + 1]] - obj[idd[k]]);
			}
			idd.clear();
			obj.clear();
			int s3 = this->m_inds.size() - id_ind;
			mergeSort(density, s2, idx, true, 0, s2 - 1, s3);
			for (size_t i = 0; i < s3; i++) {
				*m_temp_pop[id_ind] = *pop[list[idx[i]]];
				++id_ind;
			}
			density.clear();
			idx.clear();
			list.clear();
		}
		for (size_t i = 0; i < m_temp_pop.size(); i++) {
			*m_inds[i] = *m_temp_pop[i];
		}
	}

	void DCNSGAII_DE_pop::reduce_radius(int id_pro) {
		Real production = 1.0;
		for (int i = 0; i < GET_CONOP(id_pro).numVariables(); ++i) {
			production *= (GET_CONOP(id_pro).range(i).second - GET_CONOP(id_pro).range(i).first);
		}
		Real z1 = m_z / (pow(production, (1.0 / GET_CONOP(id_pro).numVariables())));
		Real C = m_R + z1;
		Real c = sqrt(abs(log2(C / z1) / log2(exp(1))));
		Real D = m_max_K / c;
		Real q = m_k / D;
		Real f = C * exp(-pow(q, 2)) - z1;
		if (abs(f) < m_Nearzero)
			f = 0.0;
		m_r = f;
	}

	void DCNSGAII_DE_pop::caculate_nichecount(std::vector<IndType*>& pop, int id_pro)
	{
		size_t size_pop = pop.size();
		std::vector<Real> nicheCount(size_pop, 0.0);
		for (int i = 0; i < size_pop; ++i) {
			for (int j = i + 1; j < size_pop; ++j) {
				Real sum1 = 0.0;
				for (int k = 0; k < GET_CONOP(id_pro).numVariables(); ++k) {
					auto difference = pop[i]->variable()[k] - pop[j]->variable()[k];
					sum1 += (difference * difference);
				}
				Real aDist = sqrt(sum1);
				if (aDist < m_R) {
					nicheCount[i] += (1 - (aDist / m_r));
					nicheCount[j] += (1 - (aDist / m_r));
				}
			}
			//pop[i]->get_vio_obj()[1] = nicheCount[i];
		}
	}

	Dominance DCNSGAII_DE_pop::e_Pareto_compare(IndType* const& s1, IndType* const& s2, int id_pro)
	{	/* One efeasible one in-efeasible */
		if (s1->get_efeasible() != s2->get_efeasible()) {
			if (s1->get_efeasible())
				return Dominance::kDominant;
			else
				return Dominance::kDominated;
		}

		/* Both efeasible */
		else if (s1->get_efeasible() && s2->get_efeasible()) {
			auto nor_obj_result = objectiveCompare<Real>(s1->objective(), s2->objective(), GET_CONOP(id_pro).optMode());
			auto vio_obj_result = objectiveCompare<Real>(s1->get_vio_obj(), s2->get_vio_obj(), OptMode::kMinimize);

			if (nor_obj_result == Dominance::kDominant && vio_obj_result == Dominance::kEqual)
				return Dominance::kDominant;
			if (nor_obj_result == Dominance::kEqual && vio_obj_result == Dominance::kDominant)
				return Dominance::kDominant;
			if (nor_obj_result == Dominance::kDominant && vio_obj_result == Dominance::kDominant)
				return Dominance::kDominant;

			if (nor_obj_result == Dominance::kDominated && vio_obj_result == Dominance::kEqual)
				return Dominance::kDominated;
			if (nor_obj_result == Dominance::kEqual && vio_obj_result == Dominance::kDominated)
				return Dominance::kDominated;
			if (nor_obj_result == Dominance::kDominated && vio_obj_result == Dominance::kDominated)
				return Dominance::kDominated;

			if (nor_obj_result == Dominance::kDominated && vio_obj_result == Dominance::kDominant)
				return Dominance::kNonDominated;
			if (nor_obj_result == Dominance::kDominant && vio_obj_result == Dominance::kDominated)
				return Dominance::kNonDominated;
			if (nor_obj_result == Dominance::kNonDominated || vio_obj_result == Dominance::kNonDominated)
				return Dominance::kNonDominated;

			if (nor_obj_result == Dominance::kEqual && vio_obj_result == Dominance::kEqual)
				return Dominance::kEqual;
		}
		//return objective_compare<Real>(s1->objective(), s2->objective(), CONTINUOUS_CAST->opt_mode());

	/* Both in-efeasible */
		else {
			if (s1->get_vio_obj()[0] < s2->get_vio_obj()[0])
				return Dominance::kDominant;
			else if (s1->get_vio_obj()[0] > s2->get_vio_obj()[0])
				return Dominance::kDominated;
			else
				return Dominance::kEqual;
		}
	}
	Dominance DCNSGAII_DE_pop::Pareto_compare(IndType* const& s1, IndType* const& s2, int id_pro)
	{
		/* Both efeasible */
		if (s1->get_efeasible() && s2->get_efeasible()) {
			auto nor_obj_result = objectiveCompare<Real>(s1->objective(), s2->objective(), GET_CONOP(id_pro).optMode());
			auto vio_obj_result = objectiveCompare<Real>(s1->get_vio_obj(), s2->get_vio_obj(), OptMode::kMinimize);

			if (nor_obj_result == Dominance::kDominant && vio_obj_result == Dominance::kEqual)
				return Dominance::kDominant;
			if (nor_obj_result == Dominance::kEqual && vio_obj_result == Dominance::kDominant)
				return Dominance::kDominant;
			if (nor_obj_result == Dominance::kDominant && vio_obj_result == Dominance::kDominant)
				return Dominance::kDominant;

			if (nor_obj_result == Dominance::kDominated && vio_obj_result == Dominance::kEqual)
				return Dominance::kDominated;
			if (nor_obj_result == Dominance::kEqual && vio_obj_result == Dominance::kDominated)
				return Dominance::kDominated;
			if (nor_obj_result == Dominance::kDominated && vio_obj_result == Dominance::kDominated)
				return Dominance::kDominated;

			if (nor_obj_result == Dominance::kDominated && vio_obj_result == Dominance::kDominant)
				return Dominance::kNonDominated;
			if (nor_obj_result == Dominance::kDominant && vio_obj_result == Dominance::kDominated)
				return Dominance::kNonDominated;
			if (nor_obj_result == Dominance::kNonDominated || vio_obj_result == Dominance::kNonDominated)
				return Dominance::kNonDominated;

			if (nor_obj_result == Dominance::kEqual && vio_obj_result == Dominance::kEqual)
				return Dominance::kEqual;
		}
	}


	void DCNSGAII_DE::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_pop.reset();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void DCNSGAII_DE::run_() {
		initPop();
		while (!terminating()) {
			m_pop->evolve(m_id_pro, m_id_alg, m_id_rnd);
		}
		m_pop->sort(m_id_pro);
	}

	void DCNSGAII_DE::initPop() {
		m_pop.reset(new DCNSGAII_DE_pop(m_pop_size, m_id_pro));
		m_pop->initialize(m_id_pro, m_id_alg, m_id_rnd);
		//m_pop->evaluate(m_id_pro, m_id_alg);
	}

	void DCNSGAII_DE::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		Real err = std::fabs(m_candidates.front()->objective(0) - GET_CONOP(m_id_pro).getOptima().objective(0).at(0));
		entry.push_back(err);
		for (size_t i = 0; i < GET_PRO(m_id_pro).numConstraints(); i++) {
			Real cons = m_candidates.front()->constraint()[i];
			entry.push_back(cons);
		}
		Real vio = (*m_pop)[0].get_vio_obj()[0];
		entry.push_back(vio);
		dynamic_cast<RecordVecReal&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}
}