#include "expanded_equal_maxima.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ExpandedEqualMaxima::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-2, 2);
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		// 5^Dim gopt 
		m_optima.clear();
		VarVec<Real> opt_var(m_num_vars);
		std::vector<Real> opt_obj(1);
		std::vector<int> quinary_code(m_num_vars);
		std::vector<Real> quinary_value = { 0.0,0.2,0.4,0.6,0.8 };
		size_t num_opts = (int)pow(5, m_num_vars);
		for (size_t i = 0; i < num_opts; i++) {
			int num = i;
			size_t j = 0;
			while (num != 0) {
				quinary_code[j++] = num % 5;
				num /= 5;
			}
			for (j = 0; j < m_num_vars; j++)
				opt_var[j] = quinary_value[quinary_code[j]];
			m_optima.appendVar(opt_var);
			evaluateObjective(opt_var.data(), opt_obj);
			m_optima.appendObj(opt_obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
	}

	void ExpandedEqualMaxima::evaluateObjective(Real *x, std::vector<Real> &obj) {
		size_t i;
		obj[0] = 0.0;
		for (i = 0; i < m_num_vars; ++i) {
			x[i] += 0.1;
			if ((x[i] <= 1.0) & (x[i] >= 0.0))
				obj[0] += -pow((sin(5 * OFEC_PI * x[i])), 6.0);
			else
				obj[0] += pow(x[i], 2.0);
		}
		obj[0] += 1.0 * m_num_vars;
	}

}