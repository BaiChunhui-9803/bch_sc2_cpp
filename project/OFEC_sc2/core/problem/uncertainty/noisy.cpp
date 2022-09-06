#include"noisy.h"

namespace ofec {
	void Noisy::initialize_() {
		m_flag_noisy_from_objective = false;
		m_obj_noisy_severity = 0.0;
		m_flag_noisy_from_variable = false;
		m_var_noisy_severity = 0.0;
		m_flag_noisy_from_environment = false;
		m_environment_noisy_severity = 0.0;
	}

	void Noisy::copy(const Problem &rP) {
		//Problem::copy(rp);
		auto &nrp = dynamic_cast<const Noisy &>(rP);

		m_flag_noisy_from_objective = nrp.m_flag_noisy_from_objective;
		m_obj_noisy_severity = nrp.m_obj_noisy_severity;
		m_flag_noisy_from_variable = nrp.m_flag_noisy_from_variable;
		m_var_noisy_severity = nrp.m_var_noisy_severity;
		m_flag_noisy_from_environment = nrp.m_flag_noisy_from_environment;
		m_environment_noisy_severity = nrp.m_environment_noisy_severity;
	}
}
