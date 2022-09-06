#include "expanded_five_uneven_peak_trap.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ExpandedFiveUnevenPeakTrap::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-40, 40);
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		// 2^Dim gopt 
		m_optima.clear();
		size_t num_opts = (int)pow(2, m_num_vars);
		VarVec<Real> opt_var(m_num_vars);
		std::vector<Real> opt_obj(1);
		std::vector<int> binary_code(m_num_vars);
		std::vector<Real> binary_value = { 0.0,30.0 };
		for (size_t i = 0; i < num_opts; i++) {
			int num = i;
			size_t j = 0;
			while (num != 0) {
				binary_code[j++] = num % 2;
				num /= 2;
			}
			for (j = 0; j < m_num_vars; j++)
				opt_var[j] = binary_value[binary_code[j]];
			m_optima.appendVar(opt_var);
			evaluateObjective(opt_var.data(), opt_obj);
			m_optima.appendObj(opt_obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
	}

	void ExpandedFiveUnevenPeakTrap::evaluateObjective(Real *x, std::vector<Real> &obj) {
		size_t i;
		obj[0] = 0.0;
		for (i = 0; i < m_num_vars; ++i) {
			if (x[i] < 0)
				obj[0] += -200.0 + pow(x[i], 2.0);
			else if (x[i] < 2.5)
				obj[0] += -80.0 * (2.5 - x[i]);
			else if (x[i] < 5.0)
				obj[0] += -64.0 * (x[i] - 2.5);
			else if (x[i] < 7.5)
				obj[0] += -160.0 + pow(x[i], 2.0);
			else if (x[i] < 12.5)
				obj[0] += -28.0 * (x[i] - 7.5);
			else if (x[i] < 17.5)
				obj[0] += -28.0 * (17.5 - x[i]);
			else if (x[i] < 22.5)
				obj[0] += -32.0 * (x[i] - 17.5);
			else if (x[i] < 27.5)
				obj[0] += -32.0 * (27.5 - x[i]);
			else if (x[i] <= 30.0)
				obj[0] += -80.0 * (x[i] - 27.5);
			else
				obj[0] += -200.0 + pow(x[i] - 30.0, 2.0);
		}
		obj[0] += 200.0 * m_num_vars;
	}

}