#include "expanded_himmelblau.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ExpandedHimmelblau::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;

		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		if (m_num_vars % 2)
			throw MyExcept("Number of variables of the expanded Himmelblau function must be an even.");
		setDomain(-10., 4.);

		// 4^(Dim/2) gopt 
		m_optima.clear();
		size_t num_opts = (int)pow(4, m_num_vars / 2);
		VarVec<Real> opt_var(m_num_vars);
		std::vector<Real> opt_obj(1);
		std::vector<int> quaternary_code(m_num_vars);
		std::vector<std::vector<Real>> quaternary_value = { { 0.0,0.0 }, {0.584428,-3.848126}, {-6.779310,-5.283186}, {-5.805118,1.131312} };
		for (size_t i = 0; i < num_opts; i++) {
			int num = i;
			size_t j = 0;
			while (num != 0) {
				quaternary_code[j++] = num % 4;
				num /= 4;
			}
			for (j = 0; j < m_num_vars / 2; j++) {
				opt_var[j * 2] = quaternary_value[quaternary_code[j]][0];
				opt_var[j * 2 + 1] = quaternary_value[quaternary_code[j]][1];
			}
			m_optima.appendVar(opt_var);
			evaluateObjective(opt_var.data(), opt_obj);
			m_optima.appendObj(opt_obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
	}

	void ExpandedHimmelblau::evaluateObjective(Real *x, std::vector<Real> &obj) {
		size_t i;
		obj[0] = 0.0;
		for (i = 0; i < m_num_vars - 1; i += 2) {
			x[i] += 3.0;
			x[i + 1] += 2.0;
			obj[0] += pow((x[i] * x[i] + x[i + 1] - 11.0), 2.0) + pow((x[i] + x[i + 1] * x[i + 1] - 7.0), 2.0);
		}
	}

}