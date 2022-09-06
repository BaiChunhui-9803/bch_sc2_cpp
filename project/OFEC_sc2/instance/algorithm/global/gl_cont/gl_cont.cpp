#include "gl_cont.h"
#include "../../../record/rcr_vec_real.h"
#include "../../../../core/problem/continuous/continuous.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void ContGL::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_update_scheme = v.count("update scheme") > 0 ? UpdateScheme(std::get<int>(v.at("update scheme"))) : UpdateScheme::bsf;
		m_alpha = v.count("alpha") > 0 ? std::get<Real>(v.at("alpha")) : 0.5;
		m_beta = v.count("beta") > 0 ? std::get<Real>(v.at("beta")) : 3.0;
		m_gamma = v.count("gamma") > 0 ? std::get<Real>(v.at("gamma")) : 6.0;
		m_pop.clear();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void ContGL::run_() {
		m_pop.resize(m_pop_size, m_id_pro);
		m_pop.setAlpha(m_alpha);
		m_pop.setBeta(m_beta);
		m_pop.setGamma(m_gamma);
		m_pop.setUpdateScheme(m_update_scheme);
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

	void ContGL::record() {
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
	void ContGL::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			m_solution[0].push_back(&m_pop[i]);
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif
}
