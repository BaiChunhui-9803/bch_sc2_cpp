#include "expanded_decreasing_minima.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ExpandedDecreasingMinima::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-40, 40);
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void ExpandedDecreasingMinima::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		size_t i;
		obj[0] = 0.0;
		for (i = 0; i < m_num_vars; ++i) {
			x[i] += 0.1;
			if ((x[i] <= 1.0) & (x[i] >= 0.0)) {
				obj[0] += -exp(-2.0 * log(2.0) * pow((x[i] - 0.1) / 0.8, 2.0)) * pow(sin(5.0 * OFEC_PI * x[i]), 6.0);
			}
			else {
				obj[0] += pow(x[i], 2.0);
			}
		}
		obj[0] += 1.0 * m_num_vars;
	}
}