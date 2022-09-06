#include "lips.h"
#include "../../../../core/instance_manager.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void LIPS::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_max_evals = v.count("maximum evaluations") > 0 ? std::get<int>(v.at("maximum evaluations")) : 1e6;
		m_pop.reset();
	}

	void LIPS::record()	{

	}

	void LIPS::run_() {
		m_pop.reset(new SwarmLIP(m_pop_size, m_id_pro, m_max_evals));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
		m_pop->initPbest(m_id_pro);
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		while (!this->terminating()) {
			m_pop->evolve(m_id_pro, m_id_alg, m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

#ifdef OFEC_DEMO
	void LIPS::updateBuffer() {
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