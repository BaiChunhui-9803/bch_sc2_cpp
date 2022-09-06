#include "crde.h"
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	PopCRDE::PopCRDE(size_t size_pop, int id_pro) : 
		PopDE(size_pop, id_pro) {}

	int PopCRDE::evolve(int id_pro, int id_alg, int id_rnd) {
		if (m_inds.size() < 5) {
			throw MyExcept("the population size cannot be smaller than 5@DE::CRDE::evolve()");
		}
		int tag = kNormalEval;
		for (size_t i = 0; i < size(); ++i) {
			mutate(i, id_rnd, id_pro);
			m_inds[i]->recombine(m_CR, m_recombine_strategy, id_rnd, id_pro);
			tag = m_inds[i]->trial().evaluate(id_pro, id_alg);
			if (tag == kTerminate) return tag;
			int idx = nearestNeighbour(i, id_pro).begin()->second;
			if (m_inds[i]->trial().dominate(*m_inds[idx], id_pro)) {
				m_inds[idx]->solut() = m_inds[i]->trial();
			}
		}
		m_iter++;
		return tag;
	}

	void CRDE::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_f = v.count("scaling factor") > 0 ? std::get<Real>(v.at("scaling factor")) : 0.5;
		m_cr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.9;
		m_ms = v.count("mutation strategy") > 0 ?
			static_cast<MutationDE>(std::get<int>(v.at("mutation strategy"))) : MutationDE::rand_1;
		m_pop.reset();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void CRDE::initPop() {
		m_pop.reset(new PopCRDE(m_pop_size, m_id_pro));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
		m_pop->CR() = m_cr;
		m_pop->F() = m_f;
		m_pop->mutationStrategy() = m_ms;
	}

	void CRDE::run_() {
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

	void CRDE::record() {
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
	void CRDE::updateBuffer() {
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

