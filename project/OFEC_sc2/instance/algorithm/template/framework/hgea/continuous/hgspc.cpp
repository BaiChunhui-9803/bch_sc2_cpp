#include "hgspc.h"
#include "hgspc_adaptor.h"
#include "../../../../../../core/algorithm/individual.h"
#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void HGSPC::initialize_() {
		HGEA::initialize_();
		auto &param = GET_PARAM(m_id_param);
		m_num_ssps = param.count("initial number of subspaces") ? 
			std::get<int>(param.at("initial number of subspaces")) : 100;
		m_adaptor.reset(new AdaptorHGSPC(m_id_pro, m_num_ssps));
		m_parents.resize(m_pop_size);
		m_num_vars = GET_PRO(m_id_pro).numVariables();
	}

#ifdef OFEC_DEMO
	void HGSPC::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1 + m_aff_inds_each_peak.size());
		for (size_t i = 0; i < m_population.size(); ++i) {
			if (m_ind_in_valley_or_peaks[i] == -1)
				m_solution[0].push_back(dynamic_cast<const Solution<>*>(m_population[i].get()));
			else
				m_solution[m_ind_in_valley_or_peaks[i] + 1].push_back(dynamic_cast<const Solution<>*>(m_population[i].get()));
		}
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

	void HGSPC::initializeSolution(size_t id_ind) {
		m_population[id_ind].reset(new Individual<>(1, 0, m_num_vars));
		m_offspring[id_ind].reset(new Individual<>(1, 0, m_num_vars));
		m_population[id_ind]->initialize(m_id_pro, m_id_rnd);
	}
}