#include "ande.h"
#include "../../../record/rcr_vec_real.h"
#include "../../../../core/instance_manager.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void ANDE::initialize_() {
		Algorithm::initialize_();
		const auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_f = v.count("scaling factor") > 0 ? std::get<Real>(v.at("scaling factor")) : 0.5;
		m_cr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.6;
	}

	void ANDE::run_() {
		m_pop.reset(new PopDE<>(m_pop_size, m_id_pro));
		m_apc.reset(new APC<IndDE>(0.9, 100, 30));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
		m_pop->CR() = m_cr;
		std::vector<size_t> ridx;
		while (!terminating()) {
			m_apc->updateData(*m_pop);
			m_apc->clustering(m_id_pro);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
			auto clusters = m_apc->clusters();
			for (auto cluster : clusters) {
				if (cluster.size() >= 4) {
					for (size_t i : cluster) {
						m_pop->selectInNeighborhood(3, cluster, ridx, m_id_rnd);
						m_pop->at(i).mutate(m_f, &m_pop->at(ridx[0]).solut(), &m_pop->at(ridx[1]).solut(), &m_pop->at(ridx[2]).solut(), m_id_pro);
						m_pop->recombine(i, m_id_rnd, m_id_pro);
						m_pop->at(i).trial().evaluate(m_id_pro, m_id_alg);
						size_t idx_nearest = cluster[0];
						Real min_dis = m_pop->at(i).trial().variableDistance(m_pop->at(idx_nearest).solut(), m_id_pro);
						Real temp_dis;
						for (size_t j = 1; j < cluster.size(); ++j) {
							temp_dis = m_pop->at(i).trial().variableDistance(m_pop->at(cluster[j]).solut(), m_id_pro);
							if (min_dis > temp_dis) {
								min_dis = temp_dis;
								idx_nearest = cluster[j];
							}
						}
						if (m_pop->at(i).trial().dominate(m_pop->at(idx_nearest).solut(), m_id_pro)) {
							m_pop->at(idx_nearest).solut() = m_pop->at(i).trial();
						}
					}
				}
				CPA(cluster);
			}
			TLLS(clusters);
		}
	}

	void ANDE::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		if (GET_PRO(m_id_pro).hasTag(ProTag::kMMOP)) {
			size_t num_optima_found = GET_PRO(m_id_pro).numOptimaFound(m_candidates);
			size_t num_optima = GET_CONOP(m_id_pro).getOptima().numberObjectives();
			entry.push_back(num_optima_found);
			entry.push_back(num_optima_found == num_optima ? 1 : 0);
		}
		else
			entry.push_back(m_candidates.front()->objectiveDistance(GET_CONOP(m_id_pro).getOptima().objective(0)));
		dynamic_cast<RecordVecReal &>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

	void ANDE::CPA(const std::vector<size_t>& cluster) {
		if (cluster.size() < 4) return;
		/* Find lbest */
		size_t lbest = cluster[0];
		for (size_t i = 0; i < cluster.size(); i++)	{
			if (m_pop->at(cluster[i]).dominate(m_pop->at(lbest), m_id_pro))
				lbest = cluster[i];
		}
		/* Find its neighbors */
		std::vector<Real> dis_to_lbest(cluster.size());
		std::vector<int> sequence;
		std::vector<int> neighbors;
		for (size_t i = 0; i < cluster.size(); i++) {
			dis_to_lbest[i] = m_pop->at(lbest).variableDistance(m_pop->at(cluster[i]), m_id_pro);
		}
		mergeSort(dis_to_lbest, dis_to_lbest.size(), sequence);
		for (size_t i = 1; i < sequence.size(); ++i) {
			neighbors.emplace_back(sequence[i]);
			if (neighbors.size() >= 5)
				break;
		}
		/* Determine the contour value f */
		Real f, f_lbest = m_pop->at(lbest).objective(0), f_i;
		if (GET_PRO(m_id_pro).optMode(0) == OptMode::kMaximize)
			f = f_lbest + 0.2 * abs(f_lbest) + 0.1;
		else
			f = f_lbest - 0.2 * abs(f_lbest) - 0.1;
		/* Calculate the interpolated points */
		size_t D = GET_CONOP(m_id_pro).numVariables();
		const auto& x_lbest = m_pop->at(lbest).variable();
		std::vector<std::vector<Real>> inters(neighbors.size(), std::vector<Real>(D));
		for (size_t i = 0; i < inters.size(); i++) {
			const auto& x_i = m_pop->at(neighbors[i]).variable();
			f_i = m_pop->at(neighbors[i]).objective(0);
			for (size_t j = 0; j < D; j++) {
				inters[i][j] = x_lbest[j] + (f - f_lbest) / (f_i - f_lbest) * (x_i[j] - x_lbest[j]);
			}
		}
		/* Estimate the potential optima */
		std::vector<Real> optima(D, 0);
		for (size_t j = 0; j < D; j++) {
			for (const auto& inter : inters) {
				optima[j] += inter[j];
			}
			optima[j] /= inters.size();
		}
		/* Compete the potential optima with the lbest */
		Solution<> temp_sol(1, 0, D);
		temp_sol.variable().vect() = optima;
		temp_sol.evaluate(m_id_pro, m_id_alg);
		if (temp_sol.dominate(m_pop->at(lbest).solut(), m_id_pro)) {
			//std::cout << "CPA works" << std::endl;
			m_pop->at(lbest).solut() = temp_sol;
		}
	}

	void ANDE::TLLS(const std::vector<std::vector<size_t>>& clusters) {
		/* Generate the sample standard deviation sigma */
		size_t D = GET_CONOP(m_id_pro).numVariables();
		Real sigma = pow(10, (-1 - (10 / (Real)D + 3) * (Real)m_effective_eval / (Real)m_MaxFEs));
		/* Calculate the niche-level search probability P */
		size_t n = clusters.size();
		std::vector<Real> P(n);
		std::vector<Real> f_niche_seed(n);
		for (size_t i = 0; i < n; i++) {
			size_t lbest = clusters[i][0];
			for (size_t k = 1; k < clusters[i].size(); k++) {
				if (m_pop->at(clusters[i][k]).dominate(m_pop->at(lbest), m_id_pro)) {
					lbest = clusters[i][k];
					f_niche_seed[i] = m_pop->at(lbest).objective(0);
				}
			}
		}
		std::vector<int> sequence;
		bool ascending = GET_PRO(m_id_pro).optMode(0) == OptMode::kMinimize;
		ascending = !ascending; // Add a exclamatory mark for sorting from worse to better
		mergeSort(f_niche_seed, n, sequence, ascending);
		std::vector<size_t> r(n);
		for (size_t i = 0; i < n; i++)
			r[sequence[i]] = i+1;
		for (size_t i = 0; i < n; i++)
			P[i] = (Real)r[i] / n;
		/*  */
		Real rand;
		for (size_t i = 0; i < n; i++) {
			rand = GET_RND(m_id_rnd).uniform.next();
			if (rand < P[i]) {
				/*  Calculater the individual-level local search probability */
				size_t n_i = clusters[i].size();
				std::vector<Real> P_i(n_i);
				std::vector<Real> f_inds_in_niche(n_i);
				for (size_t k = 0; k < n_i; k++)
					f_inds_in_niche[k] = m_pop->at(clusters[i][k]).objective(0);
				mergeSort(f_inds_in_niche, n_i, sequence, ascending);
				r.resize(n_i);
				for (size_t k = 0; k < n_i; k++)
					r[sequence[k]] = i + 1;
				for (size_t k = 0; k < n_i; k++)
					P_i[k] = (Real)r[k] / n_i;
				/* */
				for (size_t k = 0; k < n_i; k++) {
					rand = GET_RND(m_id_rnd).uniform.next();
					if (rand < P_i[k]) {
						const auto& x_ik = m_pop->at(clusters[i][k]).variable();
						Solution<> s1(1, 0, D), s2(1, 0, D);
						for (size_t j = 0; j < D; j++) {
							s1.variable()[j] = GET_RND(m_id_rnd).normal.nextNonStd(x_ik[j], sigma);
							s2.variable()[j] = GET_RND(m_id_rnd).normal.nextNonStd(x_ik[j], sigma);
						}
						s1.evaluate(m_id_pro, m_id_alg);
						s2.evaluate(m_id_pro, m_id_alg);
						s1 = s1.dominate(s2, m_id_pro) ? s1 : s2;
						if (s1.dominate(m_pop->at(clusters[i][k]).solut(), m_id_pro)) {
							//std::cout << "TLLS works" << std::endl;
							m_pop->at(clusters[i][k]).solut() = s1;
						}
					}
				}

			}
		}
	}

#ifdef OFEC_DEMO
	void ANDE::updateBuffer() {
		m_solution.clear();
		auto clusters = m_apc->clusters();
		m_solution.resize(clusters.size());
		for (size_t k = 0; k < clusters.size(); ++k) {
			for (size_t i : clusters[k]) {
				m_solution[k].push_back(&m_pop->at(i).phenotype());
			}
		}
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif
}