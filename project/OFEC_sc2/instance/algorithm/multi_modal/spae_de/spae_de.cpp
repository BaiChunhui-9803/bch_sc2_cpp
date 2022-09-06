#include "spae_de.h"
#include "../../../../utility/clustering/hslh.h"
#include "../../../../utility/clustering/apc.h"
#include "../../../../utility/clustering/fdc.h"
#include "../../../../utility/clustering/nbc.h"
#include "../../global/cma_es/cmaes_interface.h"

namespace ofec {
	void SPAE_DE::initialize_() {
		ContSPAE::initialize_();
		const auto &v = GET_PARAM(m_id_param);
		m_f = v.count("scaling factor") > 0 ? std::get<Real>(v.at("scaling factor")) : 0.5;
		m_cr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.6;
		m_ms = v.count("mutation strategy") > 0 ?
			static_cast<MutationDE>(std::get<int>(v.at("mutation strategy"))) : MutationDE::rand_1;
	}

	void SPAE_DE::initSubPop(PopDE<> &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg, int id_rnd) {
		ContSPAE::initSubPop(pop, id_atr_bsn, hlc, id_pro, id_alg, id_rnd);
		pop.F() = m_f;
		pop.CR() = m_cr;
		pop.mutationStrategy() = m_ms;
	}

	void SPAE_DE::evolveSubPop(PopDE<> &pop) {
		pop.evolve(m_id_pro, m_id_alg, m_id_rnd);
		for (size_t i = 0; i < pop.size(); ++i)
			m_hlc->inputSample(pop[i].trial());
	}

	void SPAE_DE::setPopType() {
		switch (m_ms)
		{
		case MutationDE::rand_1:
			m_pop_type = "DE-rand-1";
			break;
		case MutationDE::best_1:
			m_pop_type = "DE-best-1";
			break;
		case MutationDE::current_to_best_1:
			m_pop_type = "DE-target-to-best-1";
			break;
		case MutationDE::best_2:
			m_pop_type = "DE-best-2";
			break;
		case MutationDE::rand_2:
			m_pop_type = "DE-rand-2";
			break;
		case MutationDE::rand_to_best_1:
			m_pop_type = "DE-rand-to-best-1";
			break;
		case MutationDE::current_to_rand_1:
			m_pop_type = "DE-target-to-rand-1";
			break;
		default:
			break;
		}
	}
	
	void SPAE_RemixDE::initialize_() {
		Algorithm::initialize_();
		const auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_subpop_size = std::get<int>(v.at("subpopulation size"));
		m_hlc.reset(new HLC(GET_CONOP(m_id_pro).numVariables(), m_pop_size));
		m_hlc->initialize(GET_CONOP(m_id_pro).boundary(), GET_PRO(m_id_pro).optMode(0), 1);
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
		m_cur_bsn = -1;
		m_bsn_to_search.clear();
		m_converged.clear();
		m_divs_converged.clear();
		m_threshold = 1.0;
	}

	void SPAE_RemixDE::run_() {
		m_bsn_to_search.push_back(0);
		Real iter = 0;
		while (!terminating()) {
			/* update threshold */
			iter++;
			m_threshold = 1.0 / pow(iter, 1.0 / 4);
			for (size_t aff_bsn : m_bsn_to_search) {
				m_cur_bsn = aff_bsn;
				m_cur_stage = Stage::explore;
				initPop();
				for (size_t iter = 0; iter < 20; iter++)	{
					m_pop.evolveInBsn(*m_hlc, m_cur_bsn, m_id_pro, m_id_alg, m_id_rnd);
					for (size_t i = 0; i < m_pop.size(); i++)
						m_hlc->inputSample(m_pop[i].trial());
					
#ifdef OFEC_DEMO
					m_solution.clear();
					m_solution.resize(2);
					for (auto &sol : m_converged)
						m_solution[0].push_back(sol.get());
					for (size_t i = 0; i < m_pop.size(); ++i)
						m_solution[1].push_back(&m_pop[i]);
					Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif
				}
				/* cluster */
				m_cur_stage = Stage::cluster;
				initSubpop();
				/*  exploit */
				m_cur_stage = Stage::exploit;
				m_divs_converged.assign(m_pop_size, false);
				for (auto &sol : m_converged) {
					auto id_div = m_hlc->getIdxDiv(aff_bsn, *sol);
					if (id_div > -1)
						m_divs_converged[id_div] = true;
				}
				while (!terminating() && m_subpop.isActive()) {		
					for (size_t k = 0; k < m_subpop.size(); k++) {
						if (!m_subpop[k].isActive())
							continue;
						m_subpop[k].evolveInBsn(*m_hlc, m_cur_bsn, m_id_pro, m_id_alg, m_id_rnd);
						for (size_t i = 0; i < m_subpop[k].size(); ++i)
							//m_hlc->inputSample(m_subpop[k][i].trial());
							m_hlc->inputSample(m_subpop[k][i]);
						if (isSubpopOutBsnAtrct(k))
							m_subpop[k].setActive(false);
						if (isSubpopOverSearch(k))
							m_subpop[k].setActive(false);
						if (isSubpopConverged(k)) {
							m_subpop[k].setActive(false);
							updateConverged(k);
						}
					}
#ifdef OFEC_DEMO
					m_solution.clear();
					m_solution.resize(m_subpop.size() + 1);
					for (auto &sol : m_converged)
						m_solution[0].push_back(sol.get());
					for (size_t k = 0; k < m_subpop.size(); ++k) {
						for (size_t i = 0; i < m_subpop[k].size(); ++i)
							m_solution[k + 1].push_back(&m_subpop[k][i]);
					}
					Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif
				}
#ifdef OFEC_DEMO
				if (Demo::g_term_alg) return;
#endif
				/* Further split bains of attraction */
				std::list<Solution<> *> attractors;
				for (auto &sol : m_converged) {
					if (m_hlc->getIdxBsn(*sol) == aff_bsn)
						attractors.push_back(sol.get());
				}
				if (attractors.size() > 1)
					;
					//m_hlc->splitBasin(aff_bsn, attractors);
#ifdef OFEC_DEMO
				if (Demo::g_term_alg) return;
				m_solution.clear();
				m_solution.resize(1);
				for (auto &sol : m_converged)
					m_solution[0].push_back(sol.get());
				Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif
			}
			/* Re-estimate bains */
			m_hlc->clustering();
#ifdef OFEC_DEMO
			if (Demo::g_term_alg) return;
			m_solution.clear();
			m_solution.resize(1);
			for (auto &sol : m_converged)
				m_solution[0].push_back(sol.get());
			Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif
			/* Determine basins to search */
			m_bsn_to_search.clear();
			//m_hlc->updatePotential();
			for (size_t i = 0; i < m_hlc->numBsnsAtrct(); i++) {
				if (GET_RND(m_id_rnd).uniform.next() < m_hlc->infoBsnAtrct(i).potential)
					m_bsn_to_search.push_back(i);
			}
		}
	}

	void SPAE_RemixDE::initPop() {
		m_pop.clear();
		//m_pop.resize(m_pop_size, m_id_pro, GET_CONOP(m_id_pro).numVariables());
		m_pop.resize(m_pop_size, m_id_pro);
		for (size_t i = 0; i < m_pop.size(); i++) {
			m_hlc->randomSmpl(m_cur_bsn, m_id_rnd, m_pop[i].variable().vect());
			m_pop[i].evaluate(m_id_pro, m_id_alg);
			m_hlc->inputSample(m_pop[i]);
		}
#ifdef OFEC_DEMO
		m_solution.clear();
		m_solution.resize(2);
		for (auto &sol : m_converged)
			m_solution[0].push_back(sol.get());
		for (size_t i = 0; i < m_pop.size(); ++i)
			m_solution[1].push_back(&m_pop[i]);
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif
	}

	void SPAE_RemixDE::initSubpop() {
		//NBC<Individual<>> nbc;
		//NBC<IndDE> nbc;
		//nbc.updateData(m_pop);
		//nbc.clustering(m_id_pro);
		//auto &clusters = nbc.getClusters();
		Real pos = GET_CONOP(m_id_pro).optMode(0) == OptMode::Maximize ? 1 : -1;
		for (size_t i = 0; i < m_pop.size(); i++)
			m_pop[i].setFitness(m_pop[i].objective(0));
		FDC<IndDE> fdc(m_pop, m_id_pro);
		fdc.calFitDist();
		m_fitness_distance = fdc.getFitDist();
		fdc.clusterByDist();
		auto clusters = fdc.clusters();		
#ifdef OFEC_DEMO
		m_solution.clear();
		m_solution.resize(clusters.size());
		for (size_t k = 0; k < clusters.size(); ++k) {
			for (size_t i : clusters[k])
				m_solution[k].push_back(&m_pop[i]);
		}
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif

		//HSLH<Individual<>> hslh(m_pop, m_id_pro);
		HSLH<IndDE> hslh(m_pop, m_id_pro);
		hslh.clustering(10, m_id_pro);
		clusters = hslh.clusters();
#ifdef OFEC_DEMO
		m_solution.clear();
		m_solution.resize(clusters.size());
		for (size_t k = 0; k < clusters.size(); ++k) {
			for (size_t i : clusters[k])
				m_solution[k].push_back(&m_pop[i]);
		}
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif

		clusters = fdc.clusters();
		size_t num_vars = GET_CONOP(m_id_pro).numVariables();
		std::vector<Real> xmean(num_vars);
		std::vector<Real> stddev(num_vars);
		m_subpop.clear();
		for (auto &cluster : clusters) {
			if (cluster.size() < 2)
				continue;
			//auto subpop = std::make_unique<PopDE<>>(cluster.size(), m_id_pro);
			//for (size_t i = 0; i < cluster.size(); i++)
			//	(*subpop)[i] = m_pop[cluster[i]];
			//m_subpop.append(subpop);
			std::vector<Real> data(cluster.size());
			for (size_t j = 0; j < num_vars; j++) {
				for (size_t i = 0; i < cluster.size(); i++)
					data[i] = m_pop[cluster[i]].variable()[j];
				calMeanAndStd(data, xmean[j], stddev[j]);
				stddev[j] /= 2;
			}
			auto subpop = std::make_unique<PopExploit>(m_subpop_size, m_id_pro);
			subpop->initializeByNonStd(m_id_pro, m_id_rnd, xmean, stddev);
			subpop->evaluate(m_id_pro, m_id_alg);
			for (size_t i = 0; i < subpop->size(); i++)
				m_hlc->inputSample(subpop->at(i));
			m_subpop.append(subpop);
		}
#ifdef OFEC_DEMO
		m_solution.clear();
		m_solution.resize(m_subpop.size() + 1);
		for (auto &sol : m_converged)
			m_solution[0].push_back(sol.get());
		for (size_t k = 0; k < m_subpop.size(); ++k) {
			for (size_t i = 0; i < m_subpop[k].size(); ++i)
				m_solution[k + 1].push_back(&m_subpop[k][i]);
		}
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif
	}

	bool SPAE_RemixDE::isSubpopOutBsnAtrct(size_t k) {
		//for (size_t i = 0; i < m_subpop[k].size(); i++) {
		//	if (m_hlc->getIdxBsn(m_subpop[k][i]) != m_cur_bsn)
		//		return true;
		//}
		//return false;
		int best = 0;
		for (size_t i = 1; i < m_subpop[k].size(); i++) {
			if (m_subpop[k][i].dominate(m_subpop[k][best], m_id_pro))
				best = i;
		}
		if (m_hlc->getIdxBsn(m_subpop[k][best]) == m_cur_bsn)
			return false;
		else
			return true;
	}

	bool SPAE_RemixDE::isSubpopOverSearch(size_t k) {
		for (size_t i = 0; i < m_subpop[k].size(); i++) {
			auto id_div = m_hlc->getIdxDiv(m_cur_bsn, m_subpop[k][i]);
			if (id_div > -1 && !m_divs_converged[id_div])
				return false;
		}
		return true;
	}

	bool SPAE_RemixDE::isSubpopConverged(size_t k) {
		Real min_obj, max_obj;
		min_obj = max_obj = m_subpop[k][0].phenotype().objective(0);
		for (size_t i = 1; i < m_subpop[k].size(); i++) {
			if (m_subpop[k][i].phenotype().objective(0) < min_obj)
				min_obj = m_subpop[k][i].phenotype().objective(0);
			if (m_subpop[k][i].phenotype().objective(0) > max_obj)
				max_obj = m_subpop[k][i].phenotype().objective(0);
		}
		if ((max_obj - min_obj) < 0.1 * GET_PRO(m_id_pro).objectiveAccuracy())
			return true;
		else
			return false;
	}

	void SPAE_RemixDE::updateConverged(size_t k) {
		int best = 0;
		for (size_t i = 1; i < m_subpop[k].size(); i++) {
			if (m_subpop[k][i].dominate(m_subpop[k][best], m_id_pro))
				best = i;
		}
		/* Check distinction */
		bool distinct = true;
		for (auto &sol : m_converged) {
			if (sol->variableDistance(m_subpop[k][best], m_id_pro) < GET_CONOP(m_id_pro).variableNicheRadius()) {
				distinct = false;
				break;
			}
		}
		/* Check fitness value */
		bool fit_enough;
		if (m_converged.empty())
			fit_enough = true;
		else {
			Real pos = GET_PRO(m_id_pro).optMode(0) == OptMode::Maximize ? 1 : -1;
			Real min_fit = pos * m_hlc->worstSol().objective(0);
			Real max_fit = pos * m_hlc->bestSol().objective(0);
			Real thrd_fit = (max_fit - min_fit) * m_threshold + min_fit;
			Real new_fit = pos * m_subpop[k][best].objective(0);
			if (new_fit > thrd_fit - GET_CONOP(m_id_pro).objectiveAccuracy())
				fit_enough = true;
			else
				fit_enough = false;
		}
		if (distinct && fit_enough)
			m_converged.emplace_back(new Solution<>(m_subpop[k][best]));
		/* Remove worse converged points */
		// TODO 
	}

	void SPAE_RemixDE::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		if (GET_PRO(m_id_pro).hasTag(ProTag::MMOP)) {
			size_t num_optima_found = GET_PRO(m_id_pro).numOptimaFound(m_candidates);
			size_t num_optima = GET_CONOP(m_id_pro).getOptima().numberObjectives();
			entry.push_back(num_optima_found);
			entry.push_back(num_optima_found == num_optima ? 1 : 0);
		}
		else
			entry.push_back(m_candidates.front()->objectiveDistance(GET_CONOP(m_id_pro).getOptima().objective(0)));
		dynamic_cast<RecordVecReal &>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}
	
	void SPAE_NRandDE::setPopType() {
		m_pop_type = "DE_nrand_1";
	}

	void SPAE_NRandDE::evolveSubPop(PopNRand1DE &pop) {
		pop.evolve(m_id_pro, m_id_alg, m_id_rnd);
		for (size_t i = 0; i < pop.size(); ++i)
			m_hlc->inputSample(pop[i].trial());
	}
	
	
	int SPAE_RemixDE::PopExplore::evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		std::vector<size_t> ridx;
		size_t nearest;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			do {
				select(i, 2, ridx, id_rnd);
				auto knn = nearestNeighbour(i, id_pro, 3);
				nearest = GET_RND(id_rnd).uniform.nextElem(knn.begin(), knn.end())->second;
				m_inds[i]->mutate(m_F, m_inds[nearest].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), id_pro);
				recombine(i, id_rnd, id_pro);
			} while (hlc.getIdxBsn(m_inds[i]->trial()) != cur_bsn);
			tag = m_inds[i]->select(id_pro, id_alg);
			if (tag == EvalTag::Terminate) break;
		}
		if (tag == kNormalEval)
			m_iter++;
		return tag;
	}

	void SPAE_RemixDE::PopExploit::reproduceInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_rnd) {
		m_pop = cmaes_SamplePopulation(&m_evo, id_rnd);
		for (size_t i = 0; i < m_inds.size(); i++)
			restoreVar(m_pop[i], m_inds[i]->variable().vect(), id_pro);
		for (size_t i = 0; i < m_inds.size(); ++i) {
			while (GET_PRO(id_pro).boundaryViolated(*m_inds[i]) || hlc.getIdxBsn(*m_inds[i]) != cur_bsn) {
				cmaes_ReSampleSingle(&m_evo, i, id_rnd);
				restoreVar(m_pop[i], m_inds[i]->variable().vect(), id_pro);
			}
		}
	}

	int SPAE_RemixDE::PopExploit::evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd) {
		cmaes_UpdateDistribution(&m_evo, m_ar_funvals);
		reproduceInBsn(hlc, cur_bsn, id_pro, id_rnd);
		auto tag = evaluate(id_pro, id_alg);
		m_iter++;
		return tag;
	}

}