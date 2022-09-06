#include "ncde.h"
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	PopNCDE::PopNCDE(size_t size_pop, int id_pro) : 
		PopDE<>(size_pop, id_pro), 
		m_dis(size_pop),
		m_r(0.1),
		m_m(static_cast<size_t>(m_r * size()))
	{
		for (auto &i : m_inds) {
			i->setImproved(true);
		}
	}

	void PopNCDE::sortDistance(size_t a, int id_pro) {
		if (!m_inds[a]->isImproved()) {
			for (size_t j = 0; j < size(); ++j) {
				if (a == j) continue;
				if (m_inds[j]->isImproved()) {
					std::pair<Real, int> dis = std::make_pair(m_inds[a]->variableDistance(*m_inds[j], id_pro), j);
					auto it = m_dis[a].begin();
					while (it != m_dis[a].end() && it->first < dis.first) {
						++it;
					}
					if (it != m_dis[a].end()) {
						m_dis[a].insert(it, dis);
						m_dis[a].pop_back();
					}
				}
			}
		}
		else {
			for (size_t j = 0; j < size(); ++j) {
				if (a == j) continue;
				std::pair<Real, int> dis = std::make_pair(m_inds[a]->variableDistance(*m_inds[j], id_pro), j);
				auto it = m_dis[a].begin();
				while (it != m_dis[a].end() && it->first < dis.first) {
					it++;
				}
				if (m_dis[a].size() >= m_m) {
					if (it != m_dis[a].end()) {
						m_dis[a].insert(it, dis);
						m_dis[a].pop_back();
					}
				}
				else {
					m_dis[a].insert(it, dis);
				}
			}
		}

	}

	int PopNCDE::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		size_t nearest_index = 0;
		Real neardis;
		for (size_t i = 0; i < size(); ++i) {
			sortDistance(i, id_pro);
			std::vector<size_t> candidate;
			for (auto it = m_dis[i].begin(); it != m_dis[i].end(); it++) {
				candidate.push_back(it->second);
			}
			std::vector<size_t> result(3);
			selectInNeighborhood(3, candidate, result, id_rnd);
			m_inds[i]->mutate(m_F, m_inds[result[0]].get(), m_inds[result[1]].get(), m_inds[result[2]].get(), id_pro);
			m_inds[i]->recombine(m_CR, m_recombine_strategy, id_rnd, id_pro);
			tag = m_inds[i]->select(id_pro, id_alg);
			if (m_inds[i]->isImproved()) {
				neardis = std::numeric_limits<Real>::max();
				for (size_t j = 0; j < size(); ++j) {
					if (i == j) continue;
					Real dis = m_inds[i]->variableDistance(*m_inds[j], id_pro);
					if (neardis > dis) {
						neardis = dis;
						nearest_index = j;
					}
				}
			}
			else {
				nearest_index = m_dis[i].begin()->second;
			}
			if (m_inds[i]->dominate(*m_inds[nearest_index], id_pro)) {
				*m_inds[nearest_index] = *m_inds[i];
				m_inds[nearest_index]->setImproved(true);
			}
			if (tag != kNormalEval) break;
		}
		if (tag == kNormalEval) {
			++m_iter;
		}
		return tag;
	}


	void NCDE::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_f = v.count("scaling factor") > 0 ? std::get<Real>(v.at("scaling factor")) : 0.9;
		m_cr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.1;
		m_ms = v.count("mutation strategy") > 0 ?
			static_cast<MutationDE>(std::get<int>(v.at("mutation strategy"))) : MutationDE::rand_1;
		m_pop.reset();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void NCDE::initPop() {
		m_pop.reset(new PopNCDE(m_pop_size, m_id_pro));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
		m_pop->CR() = m_cr;
		m_pop->F() = m_f;
		m_pop->mutationStrategy() = m_ms;
	}

	void NCDE::run_() {
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

#ifdef OFEC_DEMO
	void NCDE::updateBuffer() {
		if (ofec_demo::g_buffer->idAlg() == m_id_alg) {
			m_solution.clear();
			m_solution.resize(1);
			for (size_t i = 0; i < m_pop->size(); ++i)
				m_solution[0].push_back(&m_pop->at(i).phenotype());
			ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
		}
	}
#endif

	void NCDE::record() {
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
}



