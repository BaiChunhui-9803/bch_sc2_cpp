#include "metrics_dynamic.h"
#include "../../../core/instance_manager.h"
#include "../../problem/continuous/dynamic/uncertianty_continuous.h"

namespace ofec {
	void MetricsDynamicConOEA::updateCandidates(const SolBase& sol){
		Algorithm::updateCandidates(sol);
		calculateOfflineError();
		calculateBBCError();
	}

	void MetricsDynamicConOEA::initialize_(){
		Algorithm::initialize_();
		m_offline_error = 0.;
		m_best_before_change_error = 0.;
		m_num_envirs = 0;
	}

	void MetricsDynamicConOEA::calculateOfflineError() {
		if (GET_CONOP(m_id_pro).numObjectives() == 1) {
			m_offline_error = ((GET_DYNCONOP(m_id_pro).getOptima().objective(0)[0] - m_candidates.front()->objective()[0]) + (m_effective_eval - 1) * m_offline_error) / m_effective_eval;
		}
	}

	void MetricsDynamicConOEA::calculateBBCError(){
		if (GET_CONOP(m_id_pro).numObjectives() == 1){
			if (m_effective_eval % GET_DYNCONOP(m_id_pro).getFrequency() == 0){
				m_num_envirs++;
				m_best_before_change_error = ((GET_DYNCONOP(m_id_pro).getOptima().objective(0)[0] - m_candidates.front()->objective()[0]) + (m_num_envirs - 1) * m_best_before_change_error) / m_num_envirs;
			}
		}
	}
}