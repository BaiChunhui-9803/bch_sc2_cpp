#include "cep.h"
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void CEP::initialize_() {
		Algorithm::initialize_();
		m_pop_size = std::get<int>(GET_PARAM(m_id_param).at("population size"));
		m_tau_prime = 1 / (sqrt(2 / sqrt(m_pop_size)));
		m_tau = 1 / (sqrt(2 * m_pop_size));
		m_q = 10;
		m_pop.clear();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}
	 
	void CEP::run_() {
		m_pop.resize(m_pop_size, m_id_pro);
		m_pop.tauPrime() = m_tau_prime;
		m_pop.tau() = m_tau;
		m_pop.q() = m_q;
		m_pop.initialize(m_id_pro, m_id_rnd);
		m_pop.evaluate(m_id_pro, m_id_alg);
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		while (!terminating()) {
			m_pop.evolve(m_id_pro, m_id_alg, m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

	void CEP::record() {
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
	void CEP::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			m_solution[0].push_back(&m_pop[i]);
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif
}