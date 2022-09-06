#include"GrEA.h"
#include "../../../../core/problem/continuous/continuous.h"
#include <algorithm>
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	/**********************GrEA************************/		
	void GrEA::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_pop.reset();
	}

	void GrEA::initPop() {
		auto size_var = GET_CONOP(m_id_pro).numVariables();
		auto size_obj = GET_CONOP(m_id_pro).numObjectives();
		m_pop.reset(new PopGrEA(m_pop_size, m_id_pro, size_obj));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
	}

	void GrEA::run_() {
		initPop();
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		while (!terminating()) {
			m_pop->evolve(m_id_pro,m_id_alg,m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

	void GrEA::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		Real IGD = GET_CONOP(m_id_pro).getOptima().invertGenDist(*m_pop);
		entry.push_back(IGD);
		dynamic_cast<RecordVecReal&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void GrEA::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1);
		for (size_t i = 0; i < m_pop->size(); ++i)
			m_solution[0].push_back(&m_pop->at(i).phenotype());
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

	/******************************population*************************/
	PopGrEA::PopGrEA(size_t size_pop, int id_pro, size_t size_obj) : 
		PopSBX<IndGrEA>(size_pop, id_pro),
		NSGAII(size_obj, GET_PRO(id_pro).optMode()),
		m_offspring(2 * size_pop, id_pro, GET_CONOP(id_pro).numVariables()),
		m_ind_min_max(size_obj),
		m_grid_min_max(size_obj),
		m_grid_distance(size_obj) {}

	void PopGrEA::initialize(int id_pro, int id_rnd) {
		Population<IndGrEA>::initialize(id_pro,id_rnd);
		setCR(0.9);
		setMR(1.0 / GET_CONOP(id_pro).numVariables());
		setEta(20, 20);
		gridConstruct(id_pro);
		assignGR_GCPD(id_pro);
		assignGCD(id_pro);
	}

	void PopGrEA::gridConstruct(int id_pro) {
		int num_obj = GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < num_obj; ++i) {
			m_ind_min_max[i].second = -1 * 1.0e14;
			m_ind_min_max[i].first = 1.0e14;
			for (int j = 0; j < m_inds.size(); ++j) {
				if (m_inds[j]->objective(i) > m_ind_min_max[i].second)
					m_ind_min_max[i].second = m_inds[j]->objective(i);
				if (m_inds[j]->objective(i) < m_ind_min_max[i].first)
					m_ind_min_max[i].first = m_inds[j]->objective(i);
			}
			m_grid_distance[i] = (m_ind_min_max[i].second - m_ind_min_max[i].first) * (m_grid_div + 1) / (m_grid_div * m_grid_div);
			m_grid_min_max[i].second = m_ind_min_max[i].second + (m_grid_distance[i] * m_grid_div - (m_ind_min_max[i].second - m_ind_min_max[i].first)) / 2;
			m_grid_min_max[i].first = m_ind_min_max[i].first - (m_grid_distance[i] * m_grid_div - (m_ind_min_max[i].second - m_ind_min_max[i].first)) / 2;
		}
	}

	void PopGrEA::gridConstructFi(Population<IndGrEA>& offspring, std::vector<int>& Fi, int size,int id_pro) {
		int num_obj=GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < num_obj; ++i) {
			m_ind_min_max[i].second = -1 * 1.0e14;
			m_ind_min_max[i].first = 1.0e14;
			for (int j = 0; j < size; ++j) {
				if (offspring[Fi[j]].objective()[i] > m_ind_min_max[i].second)
					m_ind_min_max[i].second = offspring[Fi[j]].objective()[i];
				if (offspring[Fi[j]].objective()[i] < m_ind_min_max[i].first)
					m_ind_min_max[i].first = offspring[Fi[j]].objective()[i];
			}
			m_grid_distance[i] = (m_ind_min_max[i].second - m_ind_min_max[i].first) * (m_grid_div + 1) / (m_grid_div * m_grid_div);
			m_grid_min_max[i].second = m_ind_min_max[i].second + (m_grid_distance[i] * m_grid_div - (m_ind_min_max[i].second - m_ind_min_max[i].first)) / 2;
			m_grid_min_max[i].first = m_ind_min_max[i].first - (m_grid_distance[i] * m_grid_div - (m_ind_min_max[i].second - m_ind_min_max[i].first)) / 2;
		}
	}

	void PopGrEA::assignGR_GCPD(int id_pro) {
		int num_obj=GET_PRO(id_pro).numObjectives();
		int flag;
		double value;
		for (int i = 0; i < m_inds.size(); i++) {
			flag = 0;
			value = 0;
			for (int j = 0; j < num_obj; j++) {
				m_inds[i]->Gk(j) = (int)floor((m_inds[i]->objective(j) - m_grid_min_max[j].first) / m_grid_distance[j]);
				flag += m_inds[i]->Gk(j);
				value += pow((m_inds[i]->objective(j) - (m_grid_min_max[j].first + m_inds[i]->Gk(j) * m_grid_distance[j])) / m_grid_distance[j], 2.0);
			}
			m_inds[i]->GR() = flag;
			m_inds[i]->GCPD() = sqrt(value);
		}
	}

	void PopGrEA::assignGR_GCPD_Fi(Population<IndGrEA>& offspring, std::vector<int>& Fi, int size,int id_pro) {
		int num_obj = GET_PRO(id_pro).numObjectives();
		int flag;
		double value;
		for (int i = 0; i < size; i++) {
			flag = 0;
			value = 0;
			for (int j = 0; j < num_obj; j++) {
				offspring[Fi[i]].Gk(j) = (int)floor((offspring[Fi[i]].objective()[j] - m_grid_min_max[j].first) / m_grid_distance[j]);
				flag += offspring[Fi[i]].Gk(j);
				value += pow((offspring[Fi[i]].objective()[j] - (m_grid_min_max[j].first + offspring[Fi[i]].Gk(j) * m_grid_distance[j])) / m_grid_distance[j], 2.0);
			}
			offspring[Fi[i]].GR() = flag;
			offspring[Fi[i]].GCPD() = sqrt(value);
		}
	}

	void PopGrEA::assignGCD(int id_pro) {
		int flag;
		int num_obj=GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < m_inds.size(); i++) {
			for (int j = i + 1; j < m_inds.size(); j++) {
				flag = 0;
				for (int k = 0; k < num_obj; k++)
					flag += abs(m_inds[i]->Gk(k) - m_inds[j]->Gk(k));
				if (flag < num_obj) {
					m_inds[i]->GCD() += num_obj - flag;
					m_inds[j]->GCD() += num_obj - flag;
				}
			}
		}
	}

	int PopGrEA::checkDominanceGrid(IndGrEA & a, IndGrEA & b, int id_pro) {
		int num_obj = GET_PRO(id_pro).numObjectives();
		int flag1 = 0, flag2 = 0;
		for (int i = 0; i < num_obj; ++i) {
			if (a.Gk(i) < b.Gk(i))
				flag1 = 1;
			else if (a.Gk(i) > b.Gk(i))
				flag2 = 1;
		}
		if (flag1 == 1 && flag2 == 0)
			return 1;
		else if (flag1 == 0 && flag2 == 1)
			return -1;
		else
			return 0;
	}

	void PopGrEA::evalEens(int id_pro, int id_rnd) {      //environment selection
		int num_obj = GET_PRO(id_pro).numObjectives();
		int pops = 0;  //indicate parent population size be 0
		int size = m_offspring.size();
		int rank = 0;
		while (true) {
			int count = 0;
			for (size_t i = 0; i < size; i++)
				if (m_offspring[i].rank() == rank)
					count++;
			int size2 = pops + count;
			if (size2 > m_inds.size()) {
				break;
			}
			for (size_t i = 0; i < size; i++)
				if (m_offspring[i].rank() == rank) {
					*(m_inds[pops]) = m_offspring[i];
					++pops;
				}
			rank++;
			if (pops >= m_inds.size()) break;
		}
		if (pops < m_inds.size())
/*				return;
		else*/ {
			//get all individual of in Fi
			std::vector<int> Fi;
			int Fi_size = 0;
			for (size_t i = 0; i < size; ++i) {
				if (m_offspring[i].rank() == rank) {
					Fi.push_back(i);
					Fi_size++;
				}
			}
			//grid construct of Fi
			gridConstructFi(m_offspring, Fi, Fi_size,id_pro);
			//assign fitness of GR and GCPD
			assignGR_GCPD_Fi(m_offspring, Fi, Fi_size,id_pro);
			for (int j = 0; j < Fi_size; ++j) {
				m_offspring[Fi[j]].GCD() = 0;
			}

			while (pops < m_inds.size()) {
				//******************find best solution (q) from, algorithm 7
				int q = 0;
				for (int k = 1; k < Fi_size; ++k) {
					if (m_offspring[Fi[k]].GR() < m_offspring[Fi[q]].GR())
						q = k;
					else if (m_offspring[Fi[k]].GR() == m_offspring[Fi[q]].GR()) {
						if (m_offspring[Fi[k]].GCD() < m_offspring[Fi[q]].GCD())
							q = k;
						else if (m_offspring[Fi[k]].GCD() == m_offspring[Fi[q]].GCD())
							if (m_offspring[Fi[k]].GCPD() < m_offspring[Fi[q]].GCPD())
								q = k;
					}
				}
				*(m_inds[pops++]) = m_offspring[Fi[q]];
				int Fi_q = Fi[q];
				Fi.erase(Fi.begin() + q);
				Fi_size--;
				//******************compute GCD of Fi, algorithm 6
				int flag;
				for (int m = 0; m < Fi_size; ++m) {
					for (int n = m + 1; n < Fi_size; ++n) {
						flag = 0;
						for (int s = 0; s < num_obj; ++s)
							flag += abs(m_offspring[Fi[m]].Gk(s) - m_offspring[Fi[n]].Gk(s));
						if (flag < num_obj) {
							m_offspring[Fi[m]].GCD() = num_obj - flag;
							m_offspring[Fi[n]].GCD() = num_obj - flag;
						}
					}
				}
				//******************adjust GR of individua in Fi, aigorithm 3
				//equal, dominated, non dominate, non equal
				int GD_q_p;
				std::vector<int> PD(Fi_size, 0);
				for (int m = 0; m < Fi_size; ++m) {
					GD_q_p = 0;
					for (int n = 0; n < num_obj; ++n) {
						GD_q_p += abs(m_offspring[Fi_q].Gk(n) - m_offspring[Fi[m]].Gk(n));
					}
					if (GD_q_p == 0)
						m_offspring[Fi[m]].GR() = num_obj + 2;
					else if (checkDominanceGrid(m_offspring[Fi_q], m_offspring[Fi[m]],id_pro) == 1)
						m_offspring[Fi[m]].GR() = num_obj;
					else {
						if (GD_q_p < num_obj) {
							if (PD[m] < (num_obj - GD_q_p)) {
								PD[m] = num_obj - GD_q_p;
								for (int n = 0; n < Fi_size; ++n)
									if ((m != n) && (checkDominanceGrid(m_offspring[Fi[m]], m_offspring[Fi[n]], id_pro) == 1) && (PD[n] < PD[m]))
										PD[n] = PD[m];
							}
						}
						m_offspring[Fi[m]].GR() = m_offspring[Fi_q].GR() + PD[m];
					}
				}
			}
		}
	}

	int PopGrEA::evolveMO(int id_pro, int id_alg, int id_rnd) {
		if (m_inds.size() % 2 != 0)
			throw "population size should be even @NSGAII_SBXRealMu::evolveMO()";
		int tag = kNormalEval;
		int m = 0;
		for (size_t n = 0; n < m_inds.size(); n += 2) {
			std::vector<int> p(2);
			p[0] = tournamentSelection(id_pro, id_rnd);
			do { p[1] = tournamentSelection(id_pro, id_rnd); } while (p[1] == p[0]);
			crossover(p[0], p[1], m_offspring[m], m_offspring[m + 1], id_pro, id_rnd);
			mutate(m_offspring[m], id_pro, id_rnd);
			mutate(m_offspring[m + 1], id_pro, id_rnd);
			tag = m_offspring[m].evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
			tag = m_offspring[m + 1].evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
			m += 2;
			m_offspring[m++] = *m_inds[n];
			m_offspring[m++] = *m_inds[n + 1];
		}
		return tag;
	}

	int PopGrEA::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		tag = evolveMO(id_pro,id_alg, id_rnd);
		nondominatedSorting(m_offspring);
		evalEens(id_pro, id_rnd);
		m_iter++;
		return tag;
	}
}