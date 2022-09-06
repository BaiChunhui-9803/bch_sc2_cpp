#include "expanded_two_peak_trap.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ExpandedTwoPeakTrap::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-40., 40.);
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void ExpandedTwoPeakTrap::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		size_t i;
		obj[0] = 0.0;
		for (i = 0; i < m_num_vars; ++i) {
			x[i] += 20.0;
			if ((x[i] < 15.0) & (x[i] >= 0.0))
				obj[0] += -(160.0 / 15.0) * (15.0 - x[i]);
			else if ((x[i] <= 20.0) & (x[i] >= 15.0))
				obj[0] += -40.0 * (x[i] - 15.0);
			else if (x[i] < 0.0)
				obj[0] += -160.0 + pow(x[i], 2.0);
			else
				obj[0] += -200.0 + pow(x[i] - 20.0, 2.0);
		}
		obj[0] += 200.0 * m_num_vars;
	}

}