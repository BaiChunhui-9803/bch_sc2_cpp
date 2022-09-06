#include"dynamic.h"
#include"../../../core/instance_manager.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif // OFEC_DEMO

namespace ofec {
	void Dynamic::updateCurState(int id_alg, bool effective_eval) {
		if (effective_eval && ID_ALG_VALID(id_alg)) {
			if (GET_ALG(id_alg).numEffectiveEvals() != 1 &&
				(GET_ALG(id_alg).numEffectiveEvals() - 1) % m_frequency == 0) {
				change();
#ifdef OFEC_DEMO
				ofec_demo::g_buffer->appendProBuffer(m_id_pro);
#endif
			}
		}
	}

	void Dynamic::updateParameters() {
		Problem::updateParameters();
		//m_params["Change frequency"] = m_frequency;
		//m_params["Change frequency:"] = m_frequency;
		//m_params["Flag time-linkage"] = m_flag_time_linkage;
		//m_params["Period"] = m_period;
	}

	int Dynamic::updateEvalTag(SolBase &s, int id_alg, bool effective_eval) {
		int rf = kNormalEval;
		if (effective_eval && ID_ALG_VALID(id_alg)) {
#ifndef OFEC_DEMO
			if (GET_ALG(id_alg).solved())
				rf |= kTerminate;
#endif // OFEC_DEMO
			if ((GET_ALG(id_alg).numEffectiveEvals()) % (m_frequency) == 0) {
				rf |= kChangeNextEval;
			}
			else if (GET_ALG(id_alg).numEffectiveEvals() != 1 &&
				(GET_ALG(id_alg).numEffectiveEvals() - 1) % m_frequency == 0) {
				rf |= kChangeCurEval;
				if (m_flag_objective_memory_change) {
					rf |= kChangeObjectiveMemory;
				}
				if (m_flag_variable_memory_change) {
					rf |= kChangeVariableMemory;
				}
			}
		}
		return rf;
	}

	void Dynamic::initialize_() {
		m_frequency = 0;
		m_period = 0;					// definite period for values repeating
		m_counter = 0;			// counter of number of changes

		m_flag_variable_memory_change = false;

		m_flag_variable_memory_dir = 0;
		m_flag_objective_memory_change = false;
		m_flag_objective_memory_dir = 0;

		m_flag_time_linkage = false;

		m_time_linkage_severity = 0.0;
		m_flag_trigger_time_linkage = false;

		m_flag_change_during_execution = false;
	}

	void Dynamic::copy(const Problem &rP) {
		//Problem::copy(rP);
		auto &drp = dynamic_cast<const Dynamic &>(rP);

		m_frequency = drp.m_frequency;
		m_period = drp.m_period;			// definite period for values repeating
		m_counter = drp.m_counter;
		m_flag_variable_memory_change = drp.m_flag_variable_memory_change;
		m_flag_variable_memory_dir = drp.m_flag_variable_memory_dir;
		m_flag_objective_memory_change = drp.m_flag_objective_memory_change;
		m_flag_objective_memory_dir = drp.m_flag_objective_memory_dir;

		m_flag_time_linkage = drp.m_flag_time_linkage;
		m_time_linkage_severity = drp.m_time_linkage_severity;
		m_flag_trigger_time_linkage = drp.m_time_linkage_severity;

		m_flag_change_during_execution = drp.m_flag_change_during_execution;
	}
}