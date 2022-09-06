#include "expanded_six_hump_camel_back.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ExpandedSixHumpCamelBack::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;

		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		if (m_num_vars % 2)
			throw MyExcept("Number of variables of the expanded six-hump camel back function must be an even.");
		for (size_t j = 0; j < m_num_vars / 2; j++) {
			m_domain.setRange(-2, 2, j * 2);
			m_domain.setRange(-0.5, 2, j * 2 + 1);
		}
		//setDomain(-2., 2.);

		// 2^(Dim/2) gopt 
		m_optima.clear();
		size_t num_opts = (int)pow(2, m_num_vars / 2);
		VarVec<Real> opt_var(m_num_vars);
		std::vector<Real> opt_obj(1);
		std::vector<int> binary_code(m_num_vars);
		std::vector<std::vector<Real>> binary_value = { { 0.0,0.0 }, {-0.179684,1.425312} };
		for (size_t i = 0; i < num_opts; i++) {
			int num = i;
			size_t j = 0;
			while (num != 0) {
				binary_code[j++] = num % 2;
				num /= 2;
			}
			for (j = 0; j < m_num_vars / 2; j++) {
				opt_var[j * 2] = binary_value[binary_code[j]][0];
				opt_var[j * 2 + 1] = binary_value[binary_code[j]][1];
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

	void ExpandedSixHumpCamelBack::evaluateObjective(Real *x, std::vector<Real> &obj) {
		size_t i;
		obj[0] = 0.0;
		for (i = 0; i<m_num_vars - 1; i += 2) {
			x[i] += 0.089842;
			x[i + 1] -= 0.712656;
			obj[0] += ((4.0 - 2.1*pow(x[i], 2.0) + pow(x[i], 4.0) / 3.0)*pow(x[i], 2.0) + x[i] * x[i + 1] + ((-4.0 + 4.0*pow(x[i + 1], 2.0))*pow(x[i + 1], 2.0)))*4.0;
		}
		obj[0] += 4.126514*m_num_vars / 2.0;
	}

}