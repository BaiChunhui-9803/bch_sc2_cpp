#include "nsde.h"
#include "../../../record/rcr_vec_real.h"
#include <algorithm>

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	PopNSDE::PopNSDE(size_t size_pop, int id_pro) : 
		PopDE<>(size_pop, id_pro),
		m_m(9), 
		m_dis((size_pop / (m_m + 1)) + 1), 
		m_seed((size_pop / (m_m + 1)) + 1) {}

	void PopNSDE::selectSubpopulation(int id_pro) {
		sort(id_pro);
		for (auto& i : m_order)
			m_order_list.emplace_back(i.second);
		int idx_species = 0;
		while (m_order_list.size() > 0) {
			m_seed[idx_species] = m_order_list[0];
			m_dis[idx_species].emplace_back(std::make_pair(0, m_order_list[0]));
			for (size_t j = 1; j < m_order_list.size(); ++j) {
				std::pair<Real, int> dis = std::make_pair(m_inds[m_seed[idx_species]]->variableDistance(*m_inds[m_order_list[j]], id_pro), m_order_list[j]);
				auto it = m_dis[idx_species].begin();
				while (it != m_dis[idx_species].end() && it->first < dis.first) {
					it++;
				}
				if (m_dis[idx_species].size() >= m_m + 1) {
					if (it != m_dis[idx_species].end()) {
						m_dis[idx_species].insert(it, dis);
						m_dis[idx_species].pop_back();
					}
				}
				else {
					m_dis[idx_species].insert(it, dis);
				}
			}
			for (auto it = m_dis[idx_species].begin(); it != m_dis[idx_species].end(); it++) {
				auto it_ = std::find(m_order_list.begin(), m_order_list.end(), it->second);
				m_order_list.erase(it_);
			}
			++idx_species;
		}
	}

	int PopNSDE::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		Real u, l;
		for (size_t i = 0; i < m_dis.size(); ++i) {
			for (auto it = m_dis[i].begin(); it != m_dis[i].end(); it++) {
				mutate(it->second, id_rnd, id_pro);
				m_inds[it->second]->recombine(m_CR, m_recombine_strategy, id_rnd, id_pro);
				tag = m_inds[it->second]->select(id_pro, id_alg);
				if (m_seed[i] != it->second) {
					if (m_inds[m_seed[i]]->same(*m_inds[it->second], id_pro)) {
						for (size_t k = 0; k < m_inds[it->second]->variable().size(); ++k) {
							l = GET_CONOP(id_pro).range(k).first;
							u = GET_CONOP(id_pro).range(k).second;

							m_inds[it->second]->variable()[k] = GET_RND(id_rnd).uniform.nextNonStd<Real>(l, u);
						}
						m_inds[it->second]->evaluate(id_pro, id_alg);
					}
					else if (m_inds[it->second]->dominate(*m_inds[m_seed[i]], id_pro)) {
						*m_inds[m_seed[i]] = *m_inds[it->second];
					}
				}
				if (tag != kNormalEval) break;
			}
			if (tag != kNormalEval) break;
		}
		++m_iter;
		return tag;
	}

	void NSDE::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_f = v.count("scaling factor") > 0 ? std::get<Real>(v.at("scaling factor")) : 0.5;
		m_cr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.7;
		m_ms = v.count("mutation strategy") > 0 ?
			static_cast<MutationDE>(std::get<int>(v.at("mutation strategy"))) : MutationDE::rand_1;
		m_pop.reset();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void NSDE::initPop() {
		m_pop.reset(new PopNSDE(m_pop_size, m_id_pro));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
		m_pop->CR() = m_cr;
		m_pop->F() = m_f;
		m_pop->mutationStrategy() = m_ms;
		m_pop->selectSubpopulation(m_id_pro);
	}

	void NSDE::run_() {
		initPop();
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		while (!terminating()) {
			m_pop->evolve(m_id_pro, m_id_alg, m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

	void NSDE::record() {
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

#ifdef OFEC_DEMO
	void NSDE::updateBuffer() {
		if (ofec_demo::g_buffer->idAlg() == m_id_alg) {
			m_solution.clear();
			m_solution.resize(1);
			for (size_t i = 0; i < m_pop->size(); ++i)
				m_solution[0].push_back(&m_pop->at(i).phenotype());
			ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
		}
	}
#endif
}


