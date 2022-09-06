#include "sade.h"
#include <algorithm>

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void SaDE::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_pop.reset();
	}

	void SaDE::run_() {
		m_pop.reset(new PopSaDE(m_pop_size, m_id_pro));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
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

	void SaDE::record() {

	}


#ifdef OFEC_DEMO
	void SaDE::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1);
		for (size_t i = 0; i < m_pop->size(); ++i)
			m_solution[0].push_back(&m_pop->at(i).phenotype());
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

}
