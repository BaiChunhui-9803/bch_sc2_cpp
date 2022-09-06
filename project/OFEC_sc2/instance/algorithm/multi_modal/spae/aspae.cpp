#include "aspae.h"
#include "../../global/cma_es/cmaes_interface.h"
#include <numeric>
#include "../../../record/rcr_vec_real.h"
#include "../../../../utility/clustering/hslh.h"
#include "../../../../utility/clustering/fdc.h"
#include "../../../../utility/clustering/nbc.h"
#include "../../../../utility/clustering/dpc.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif // OFEC_DEMO


namespace ofec {
	void ASPAE::initialize_() {
		Algorithm::initialize_();
		const auto &v = GET_PARAM(m_id_param);
		m_num_ssps = std::get<int>(v.at("number of subspaces"));
		m_pop_size = std::get<int>(v.at("population size"));
		m_subpop_size = std::get<int>(v.at("subpopulation size"));
		m_hlc.reset();
		m_cur_bsn = -1;
		//m_converg_radius = 1e-6;
		m_main_pop.clear();
		m_subpops.clear();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void ASPAE::run_() {
		m_hlc.reset(new HLC(GET_CONOP(m_id_pro).numVariables(), m_pop_size));
		//m_hlc.reset(new HLC(GET_CONOP(m_id_pro).numVariables(), 0));
		m_hlc->initialize(GET_CONOP(m_id_pro).boundary(), GET_PRO(m_id_pro).optMode(0), 1);
		while (!terminating()) {
			{
				//std::vector<Real> pots(m_hlc->numBsnsAtrct());
				//for (size_t i = 0; i < m_hlc->numBsnsAtrct(); ++i)
				//	pots[i] = pow(m_hlc->infoBsnAtrct(i).potential, 2);
				//auto select = GET_RND(m_id_rnd).uniform.spinWheel(pots.begin(), pots.end());
				//size_t cur_bsn = select - pots.begin();

				size_t cur_bsn = 0;
				for (size_t i = 1; i < m_hlc->numBsnsAtrct(); ++i)
					if (m_hlc->infoBsnAtrct(i).potential > m_hlc->infoBsnAtrct(cur_bsn).potential)
						cur_bsn = i;

				m_main_pop.clear();
				m_main_pop.resize(m_pop_size, m_id_pro);
				for (size_t i = 0; i < m_main_pop.size(); ++i) {
					m_hlc->randomSmplByFreq(cur_bsn, m_id_rnd, m_main_pop[i].variable().vect());
					//m_hlc->randomSmpl(cur_bsn, m_id_rnd, m_main_pop[i].variable().vect());
					m_main_pop[i].evaluate(m_id_pro, m_id_alg);
					m_hlc->inputSample(m_main_pop[i]);
				}
#ifdef OFEC_DEMO
				m_solution.clear();
				m_solution.resize(1);
				for (size_t i = 0; i < m_main_pop.size(); i++)
					m_solution[0].push_back(&m_main_pop[i].solut());
				Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif // OFEC_DEMO

				while (!terminating() && m_main_pop.iteration() < 20) {
					m_main_pop.evolveInBsn(*m_hlc, cur_bsn, m_id_pro, m_id_alg, m_id_rnd, m_attractors_fnd);
					for (size_t i = 0; i < m_main_pop.size(); i++)
						m_hlc->inputSample(m_main_pop[i].trial());
#ifdef OFEC_DEMO
					for (size_t i = 0; i < m_main_pop.size(); i++)
						m_solution[0][i] = &m_main_pop[i].solut();
					Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif // OFEC_DEMO
				}

				std::vector<const Solution<>*> sols(m_main_pop.size());
				for (size_t i = 0; i < m_main_pop.size(); ++i)
					sols[i] = &m_main_pop[i].solut();
				m_hlc->updateDPC(cur_bsn, sols, m_id_pro, m_id_rnd);
				m_cur_bsn = cur_bsn;

				DPC dpc;
				dpc.updateData(m_main_pop, m_id_pro);
				dpc.clustering(0.01,0.3);
				auto &clusters = dpc.clusters();

				//HSLH<IndDE> hslh(m_main_pop, m_id_pro);
				//hslh.clustering(5, m_id_pro);
				//auto &clusters = hslh.clusters();

				//m_main_pop.updateFitness(m_id_pro);
				//FDC<IndDE> fdc(m_main_pop, m_id_pro);
				//fdc.clusterByDist(0.15);
				//auto &clusters = fdc.clusters();

				//NBC<IndDE> nbc;
				//nbc.updateData(m_main_pop);
				//nbc.clustering(m_id_pro);
				//auto &clusters = nbc.clusters();

#ifdef OFEC_DEMO
				m_solution.clear();
				m_solution.resize(clusters.size());
				for (size_t k = 0; k < clusters.size(); ++k) {
					for (size_t i = 0; i < clusters[k].size(); ++i) {
						m_solution[k].push_back(&m_main_pop[clusters[k][i]]);
					}
				}
				Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif // OFEC_DEMO
				m_hlc->clearTypInds();
				m_cur_bsn = -1;

				m_subpops.clear();
				m_centers.clear();
				for (auto &center : m_centers)
					center.resize(GET_CONOP(m_id_pro).numVariables());
				m_radius.clear();
				m_init_radius.clear();

				std::list<size_t> sole_clstrs;
				size_t id_subpop = 0;
				for (auto &cluster : clusters) {
					if (cluster.size() < 2) {
						sole_clstrs.push_back(cluster.front());
						continue;
					}
					auto new_sub_pop = std::make_unique<PopExploit>(m_subpop_size, m_id_pro);
					m_subpops.append(new_sub_pop);
					initSubpop(id_subpop, cluster);
					m_subpops[id_subpop].setId(0);
					m_subpops[id_subpop].updateBest(m_id_pro);
					m_centers.push_back(VarVec<Real>(GET_CONOP(m_id_pro).numVariables()));
					m_radius.resize(id_subpop + 1);
					m_init_radius.resize(id_subpop + 1);
					updateCenterRadius(id_subpop);
					id_subpop++;
				}
				//Real max_radius = m_radius[0];
				//for (size_t k = 1; k < m_radius.size(); ++k)
				//	if (m_radius[k] > max_radius)
				//		max_radius = m_radius[k];
				//for (size_t clstr : sole_clstrs) {
				//	auto new_sub_pop = std::make_unique<PopExploit>(m_subpop_size, m_id_pro);
				//	m_subpops.append(new_sub_pop);
				//	m_subpops[id_subpop][0] = m_main_pop[clstr];
				//	for (size_t i = 1; i < m_subpop_size; ++i) {
				//		for (size_t j = 0; j < GET_CONOP(m_id_pro).numVariables(); ++j)
				//			m_subpops[id_subpop][i].variable()[j] = GET_RND(m_id_rnd).normal.nextNonStd(
				//				m_main_pop[clstr].variable()[j], max_radius / 2);
				//		GET_CONOP(m_id_pro).validateSolution(m_subpops[id_subpop][i], Validation::SetToBound, m_id_rnd);
				//		m_subpops[id_subpop][i].evaluate(m_id_pro, m_id_alg);
				//		m_hlc->inputSample(m_subpops[id_subpop][i]);
				//	}
				//	m_subpops[id_subpop].setId(0);
				//	m_subpops[id_subpop].updateBest(m_id_pro);
				//	m_centers.push_back(m_main_pop[clstr].variable());
				//	m_radius.push_back(max_radius);
				//	m_init_radius.push_back(max_radius);
				//	id_subpop++;
				//}
#ifdef OFEC_DEMO
				m_solution.clear();
				m_solution.resize(m_subpops.size());
				for (size_t k = 0; k < m_subpops.size(); ++k) {
					for (size_t i = 0; i < m_subpops[k].size(); ++i) {
						m_solution[k].push_back(&m_subpops[k][i]);
					}
				}
				Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif // OFEC_DEMO

				while (!terminating() && m_subpops.isActive()) {
					for (size_t k = 0; k < m_subpops.size(); ++k) {
						if (!m_subpops[k].isActive())
							continue;
						//m_subpops[k].evolveInBsn(*m_hlc, cur_bsn, m_id_pro, m_id_alg, m_id_rnd);
						m_subpops[k].evolve(m_id_pro, m_id_alg, m_id_rnd);
						updateCenterRadius(k);
						for (size_t i = 0; i < m_subpops[k].size(); i++)
							m_hlc->inputSample(m_subpops[k][i].trial());
						std::vector<const Solution<>*> subpop(m_subpops[k].size());
						for (size_t i = 0; i < m_subpops[k].size(); i++)
							subpop[i] = &m_subpops[k][i];
						m_hlc->updateImprovedSample(subpop);
						m_subpops[k].updateBest(m_id_pro);
						if (m_hlc->getIdxBsn(*m_subpops[k].best().front()) != cur_bsn)
							m_subpops[k].setActive(false);
						//if (isSubpopConverged(k)) {
						//	if (m_subpops[k].id() == 0) {
						//		disturbSubPop(k);
						//		m_subpops[k].setId(1);
						//	}
						//	else
						//		m_subpops[k].setActive(false);
						//}
						if (m_subpops[k].iteration() - m_subpops[k].timeBestUpdated() > 20)
							m_subpops[k].setActive(false);
						if (isSubpopConverged(k)) {
							m_subpops[k].setActive(false);
							updateAttrctrsFnd(k);
							//auto pre_best = *m_subpops[k].best().front();
							//auto cur_best = *m_subpops[k].best(m_id_pro).front();
							//if (cur_best.dominate(pre_best, m_id_pro))
							//	m_subpops[k].setId(m_subpops[k].id() + 1);
							//else
							//	m_subpops[k].setId(m_subpops[k].id() + 2);
							//if (GET_RND(m_id_rnd).uniform.next() < 1.0 / m_subpops[k].id())
							//	disturbSubPop(k);
							//else {
							//	m_subpops[k].setActive(false);
							//	updateAttrctrsFnd(k);
							//}
						}
					}
					for (size_t k1 = 0; k1 < m_subpops.size(); ++k1) {
						for (size_t k2 = k1 + 1; k2 < m_subpops.size(); ++k2) {
							if (!m_subpops[k2].isActive())
								continue;
							if (!isOverlapping(k1, k2))
								continue;
							mergeSubpops(k1, k2);
							m_subpops[k2].setActive(false);
						}
					}

#ifdef OFEC_DEMO
					m_solution.clear();
					for (size_t k = 0; k < m_subpops.size(); ++k) {
						if (!m_subpops[k].isActive())
							continue;
						m_solution.resize(m_solution.size() + 1);
						for (size_t i = 0; i < m_subpops[k].size(); ++i) {
							m_solution.back().push_back(&m_subpops[k][i]);
						}
					}
					Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif // OFEC_DEMO
				}

				std::vector<const Solution<> *> atrcts;
				for (auto &atrct : m_attractors_fnd)
					atrcts.push_back(&atrct);
				m_hlc->splitSubspaces(atrcts);
#ifdef OFEC_DEMO
				m_solution.clear();
				m_solution.resize(1);
				for (auto &atrct : m_attractors_fnd)
					m_solution.back().push_back(&atrct);
				Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif // OFEC_DEMO

				m_hlc->clustering();
#ifdef OFEC_DEMO
				Demo::g_buffer->appendAlgBuffer(m_id_alg);
#endif // OFEC_DEMO

				m_hlc->updatePotential(m_id_pro, m_id_rnd, m_attractors_fnd);
#ifdef OFEC_DEMO
				Demo::g_buffer->appendAlgBuffer(m_id_alg);
				for (size_t id_bsn = 0; id_bsn < m_hlc->numBsnsAtrct(); ++id_bsn) {
					m_cur_bsn = id_bsn;
					Demo::g_buffer->appendAlgBuffer(m_id_alg);
				}
				m_cur_bsn = -1;
#endif // OFEC_DEMO
				m_hlc->clearTypInds();
			}
		}
	}

	void ASPAE::initSubpop(size_t k, const std::vector<size_t> &cluster) {
		if (cluster.size() > m_subpop_size) {
			std::vector<size_t> seq(cluster);
			std::sort(seq.begin(), seq.end(),
				[&](size_t i1, size_t i2) {return m_main_pop[i1].dominate(m_main_pop[i1], m_id_pro); });
			for (size_t i = 0; i < m_subpop_size; ++i)
				m_subpops[k][i] = m_main_pop[seq[i]];
		}
		else {
			for (size_t i = 0; i < cluster.size(); ++i)
				m_subpops[k][i] = m_main_pop[cluster[i]];
			if (cluster.size() < m_subpop_size) {
				size_t num_vars = GET_CONOP(m_id_pro).numVariables();
				std::vector<Real> mean(num_vars), stddev(num_vars), data(cluster.size());
				for (size_t j = 0; j < num_vars; ++j) {
					for (size_t i = 0; i < cluster.size(); ++i)
						data[i] = m_main_pop[cluster[i]].variable()[j];
					calMeanAndStd(data, mean[j], stddev[j]);
				}
				for (size_t i = cluster.size(); i < m_subpop_size; ++i) {
					for (size_t j = 0; j < num_vars; ++j)
						m_subpops[k][i].variable()[j] = GET_RND(m_id_rnd).normal.nextNonStd(mean[j], stddev[j] / 3);
					GET_CONOP(m_id_pro).validateSolution(m_subpops[k][i], Validation::SetToBound,m_id_rnd);
					m_subpops[k][i].evaluate(m_id_pro, m_id_alg);
					m_hlc->inputSample(m_subpops[k][i]);
				}
			}
		}
	}

	void ASPAE::updateCenterRadius(size_t k) {
		m_centers[k] = m_subpops[k][0].variable();
		for (size_t j = 0; j < GET_CONOP(m_id_pro).numVariables(); ++j) {
			for (size_t i = 1; i < m_subpops[k].size(); ++i)
				m_centers[k][j] += m_subpops[k][i].variable()[j];
			m_centers[k][j] /= m_subpops[k].size();
		}
		m_radius[k] = GET_PRO(m_id_pro).variableDistance(m_centers[k], m_subpops[k][0].variable());
		for (size_t i = 1; i < m_subpops[k].size(); ++i)
			m_radius[k] += GET_PRO(m_id_pro).variableDistance(m_centers[k], m_subpops[k][i].variable());
		m_radius[k] /= m_subpops[k].size();
		if (m_subpops[k].iteration() == 0)
			m_init_radius[k] = m_radius[k];
	}

	bool ASPAE::isOverlapping(size_t k1, size_t k2) {
		Real center_dis = GET_PRO(m_id_pro).variableDistance(m_centers[k1], m_centers[k2]);
		Real dis(0);
		if (center_dis < m_radius[k1] && center_dis < m_radius[k2]) {
			int c1(0), c2(0);
			for (size_t i = 0; i < m_subpops[k2].size(); ++i) {
				dis = GET_PRO(m_id_pro).variableDistance(m_centers[k1], m_subpops[k2][i].variable());
				if (dis < m_radius[k1]) c1++;
			}
			for (size_t i = 0; i < m_subpops[k1].size(); ++i) {
				dis = GET_PRO(m_id_pro).variableDistance(m_centers[k2], m_subpops[k1][i].variable());
				if (dis < m_radius[k2]) c2++;
			}
			return c1 > 0 && c2 > 0;
		}
		else
			return false;
	}

	void ASPAE::mergeSubpops(size_t k1, size_t k2) {
		std::vector<Solution<>*> sols;
		for (size_t i = 0; i < m_subpops[k1].size(); ++i)
			sols.push_back(&m_subpops[k1][i]);
		for (size_t i = 0; i < m_subpops[k2].size(); ++i)
			sols.push_back(&m_subpops[k2][i]);
		std::vector<size_t> idxs(sols.size());
		std::iota(idxs.begin(), idxs.end(), 0);
		std::sort(idxs.begin(), idxs.end(),
			[&](size_t i1, size_t i2) {
				return sols[i1]->dominate(*sols[i2], m_id_pro);
			});
		std::list<size_t> survive_k2;
		std::vector<bool> survive_k1(m_subpops[k1].size(), false);
		for (size_t i = 0; i < m_subpops[k1].size(); ++i) {
			if (idxs[i] < m_subpops[k1].size())
				survive_k1[idxs[i]] = true;
			else
				survive_k2.push_back(idxs[i] - m_subpops[k1].size());
		}
		for (size_t i = 0; i < m_subpops[k1].size(); ++i) {
			if (!survive_k1[i]) {
				m_subpops[k1][i] = m_subpops[k2][survive_k2.front()];
				survive_k2.pop_front();
			}
		}
	}

	//bool ASPAE::isSubpopOverSearch(size_t k) {
	//	return false;
	//}

	bool ASPAE::isSubpopConverged(size_t k) {
		//if (m_radius[k] < m_converg_radius)
		//	return true;
		//else
		//	return false;

		Real min_obj, max_obj;
		min_obj = max_obj = m_subpops[k][0].phenotype().objective(0);
		for (size_t i = 1; i < m_subpops[k].size(); i++) {
			if (m_subpops[k][i].phenotype().objective(0) < min_obj)
				min_obj = m_subpops[k][i].phenotype().objective(0);
			if (m_subpops[k][i].phenotype().objective(0) > max_obj)
				max_obj = m_subpops[k][i].phenotype().objective(0);
		}
		if ((max_obj - min_obj) < 0.1 * GET_PRO(m_id_pro).objectiveAccuracy()) {
			return true;
		}
		else
			return false;
	}

	void ASPAE::disturbSubPop(size_t k) {
		//size_t best = 0;
		//for (size_t i = 1; i < m_subpops[k].size(); ++i) {
		//	if (m_subpops[k][i].dominate(m_subpops[k][best], m_id_pro))
		//		best = i;
		//}
		//for (size_t j = 0; j < GET_CONOP(m_id_pro).numVariables(); j++)
		//	m_subpops[k][best].variable()[j] = GET_RND(m_id_rnd).uniform.nextNonStd(
		//		m_subpops[k][best].variable()[j] - m_init_radius[k], m_subpops[k][best].variable()[j] + m_init_radius[k]);
		//GET_CONOP(m_id_pro).validateSolution(m_subpops[k][best], Validation::SetToBound, m_id_rnd);
		//m_subpops[k][best].evaluate(m_id_pro, m_id_alg);
		//m_hlc->inputSample(m_subpops[k][best]);

		for (size_t i = 1; i < m_subpops[k].size(); ++i) {
			if (i % 2)
				continue;
			for (size_t j = 0; j < GET_CONOP(m_id_pro).numVariables(); ++j)
				m_subpops[k][i].variable()[j] = GET_RND(m_id_rnd).uniform.nextNonStd(
					m_subpops[k][i].variable()[j] - m_init_radius[k], m_subpops[k][i].variable()[j] + m_init_radius[k]);
			GET_CONOP(m_id_pro).validateSolution(m_subpops[k][i], Validation::SetToBound, m_id_rnd);
			m_subpops[k][i].evaluate(m_id_pro, m_id_alg);
			m_hlc->inputSample(m_subpops[k][i]);
		}
	}

	void ASPAE::updateAttrctrsFnd(size_t k) {
		size_t best = 0;
		for (size_t i = 1; i < m_subpops[k].size(); ++i) {
			if (m_subpops[k][i].dominate(m_subpops[k][best], m_id_pro))
				best = i;
		}
		bool distinct = true;
		for (auto &atrct_fnd : m_attractors_fnd) {
			if (atrct_fnd.variableDistance(m_subpops[k][best], m_id_pro) < GET_CONOP(m_id_pro).variableNicheRadius()) {
				if (m_subpops[k][best].dominate(atrct_fnd, m_id_pro))
					atrct_fnd = m_subpops[k][best];
				distinct = false;
				break;
			}
		}
		if (distinct)
			m_attractors_fnd.emplace_back(m_subpops[k][best]);
	}

	void ASPAE::recordCovSSPs() {
		std::vector<const Solution<>*> sols(m_main_pop.size());
		for (size_t i = 0; i < sols.size(); ++i)
			sols[i] = &m_main_pop[i].solut();
		m_hlc->recordCovSSPs(m_cur_bsn, sols);
	}

	void ASPAE::record() {
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

	int ASPAE::PopExplore::evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd, const std::list<Solution<>> &attractors_fnd) {
		int tag = kNormalEval;
		std::vector<size_t> ridx;
		size_t nearest;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			do {
				select(i, 2, ridx, id_rnd);
				auto knn = nearestNeighbour(i, id_pro, 2);
				nearest = GET_RND(id_rnd).uniform.nextElem(knn.begin(), knn.end())->second;
				m_inds[i]->mutate(m_F, m_inds[nearest].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), id_pro);
				/* repel trial vector from attractors found so far */
				//Real strength;
				//size_t size_var = GET_CONOP(id_pro).numVariables();
				//for (auto &atrct : attractors_fnd) {
				//	strength = abs(GET_RND(id_rnd).normal.nextNonStd(0, 1.0 / 3));
				//	for (size_t j = 0; j < size_var; j++) {
				//		m_inds[i]->donor().variable()[j] += strength * (m_inds[i]->donor().variable()[j] - atrct.variable()[j]);
				//	}
				//}
				//for (size_t j = 0; j < size_var; j++) {
				//	Real l = GET_CONOP(id_pro).range(j).first;
				//	Real u = GET_CONOP(id_pro).range(j).second;
				//	if ((m_inds[i]->donor().variable()[j]) > u) {
				//		m_inds[i]->donor().variable()[j] = ((m_inds[i]->donor().variable()[j]) + u) / 2;
				//	}
				//	else if ((m_inds[i]->donor().variable()[i]) < l) {
				//		m_inds[i]->donor().variable()[j] = ((m_inds[i]->donor().variable()[j]) + l) / 2;
				//	}
				//}
				recombine(i, id_rnd, id_pro);
			} while (/*GET_RND(id_rnd).uniform.next() > 1.0 / (m_iter + 1)
				&& */hlc.getIdxBsn(m_inds[i]->trial()) != cur_bsn);
			tag = m_inds[i]->select(id_pro, id_alg);
			if (tag == EvalTag::Terminate) break;
		}
		if (tag == kNormalEval)
			m_iter++;
		return tag;
	}

	int ASPAE::PopExploit::evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		std::vector<size_t> ridx;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			do {
				select(i, 3, ridx, id_rnd);
				m_inds[i]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro);
				recombine(i, id_rnd, id_pro);
			} while (GET_RND(id_rnd).uniform.next() > 1.0 / (m_iter + 1) 
				&& hlc.getIdxBsn(m_inds[i]->trial()) != cur_bsn);
			tag = m_inds[i]->select(id_pro, id_alg);
			if (tag == EvalTag::Terminate) break;
		}
		if (tag == kNormalEval)
			m_iter++;
		return tag;
	}

	//void PopExploit::reproduceInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_rnd) {
	//	m_pop = cmaes_SamplePopulation(&m_evo, id_rnd);
	//	for (size_t i = 0; i < m_inds.size(); i++)
	//		restoreVar(m_pop[i], m_inds[i]->variable().vect(), id_pro);
	//	for (size_t i = 0; i < m_inds.size(); ++i) {
	//		while (GET_PRO(id_pro).boundaryViolated(*m_inds[i]) || hlc.getIdxBsn(*m_inds[i]) != cur_bsn) {
	//			cmaes_ReSampleSingle(&m_evo, i, id_rnd);
	//			restoreVar(m_pop[i], m_inds[i]->variable().vect(), id_pro);
	//		}
	//	}
	//}

	//void PopExploit::initializeInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_rnd, const std::vector<Real> &xstart, const std::vector<Real> &stddev) {
	//	std::vector<double> xstart_(xstart.size()), stddev_(stddev.size());
	//	normVar(xstart, xstart_.data(), id_pro);
	//	normDis(stddev, stddev_.data(), id_pro);
	//	m_ar_funvals = cmaes_init(&m_evo, GET_CONOP(id_pro).numVariables(), xstart_.data(), stddev_.data(), 0, m_inds.size(), 0, id_rnd);
	//	reproduceInBsn(hlc, cur_bsn, id_pro, id_rnd);
	//}

	//int PopExploit::evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd) {
	//	cmaes_UpdateDistribution(&m_evo, m_ar_funvals);
	//	reproduceInBsn(hlc, cur_bsn, id_pro, id_rnd);
	//	auto tag = evaluate(id_pro, id_alg);
	//	m_iter++;
	//	return tag;
	//}
}