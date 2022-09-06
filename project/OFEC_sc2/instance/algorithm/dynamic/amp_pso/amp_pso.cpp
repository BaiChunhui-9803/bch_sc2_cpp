#include "amp_pso.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void AMP_PSO::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_w = v.count("weight") > 0 ? std::get<Real>(v.at("weight")) : 0.7298;
		m_c1 = v.count("accelerator1") > 0 ? std::get<Real>(v.at("accelerator1")) : 1.496;
		m_c2 = v.count("accelerator2") > 0 ? std::get<Real>(v.at("accelerator2")) : 1.496;
		m_multi_pop.reset();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
		m_obj_minmax_monitored = true;
	}

	void AMP_PSO::record() {}

	void AMP_PSO::initMultiPop() {
		m_multi_pop.reset(new ContAMP<PopType>(m_pop_size));
		m_multi_pop->initialize(m_id_pro, m_id_alg, m_id_rnd);
		for (size_t k = 0; k < m_multi_pop->size(); k++) {
			m_multi_pop->at(k).W() = m_w;
			m_multi_pop->at(k).C1() = m_c1;
			m_multi_pop->at(k).C1() = m_c2;
			m_multi_pop->at(k).initPbest(m_id_pro);
		}
#ifdef OFEC_DEMO
		updateBuffer();
#endif
	}

	void AMP_PSO::run_() {
		initMultiPop();
		while (!terminating()) {
			m_multi_pop->evolve(m_id_pro, m_id_alg, m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

#ifdef OFEC_DEMO
	void AMP_PSO::updateBuffer() {
		m_solution.clear();
		m_solution.resize(m_multi_pop->size());
		for (size_t k = 0; k < m_multi_pop->size(); k++) {
			for (size_t i = 0; i < m_multi_pop->at(k).size(); ++i)
				m_solution[k].push_back(&m_multi_pop->at(k)[i].phenotype());
		}
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif
}