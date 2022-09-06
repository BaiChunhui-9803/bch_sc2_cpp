#include "amp_de.h"
#include "../../../../core/instance_manager.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void AMP_DE::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_f = v.count("scaling factor") > 0 ? std::get<Real>(v.at("scaling factor")) : 0.5;
		m_cr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.6;
		m_ms = v.count("mutation strategy") > 0 ?
			static_cast<MutationDE>(std::get<int>(v.at("mutation strategy"))) : MutationDE::rand_1;
		m_multi_pop.reset();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
		m_obj_minmax_monitored = true;
	}

	void AMP_DE::record() {}

	void AMP_DE::initMultiPop()	{
		m_multi_pop.reset(new ContAMP<PopType>(m_pop_size));
		m_multi_pop->initialize(m_id_pro, m_id_alg, m_id_rnd);
		for (size_t k = 0; k < m_multi_pop->size(); k++) {
			m_multi_pop->at(k).F() = m_f;
			m_multi_pop->at(k).CR() = m_cr;
			m_multi_pop->at(k).mutationStrategy() = m_ms;
		}
#ifdef OFEC_DEMO
		updateBuffer();
#endif
	}

	void AMP_DE::run_() {
		initMultiPop();
		while (!terminating()) {
			m_multi_pop->evolve(m_id_pro, m_id_alg, m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

#ifdef OFEC_DEMO
	void AMP_DE::updateBuffer() {
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