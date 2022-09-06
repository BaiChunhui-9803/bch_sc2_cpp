#include "shubert.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Shubert::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-10, 10);
		setInitialDomain(-10, 10);
		m_variable_accuracy = 0.5;
		m_optima.clear();
		size_t num = m_num_vars*(size_t)pow(3, m_num_vars);	
		if (m_num_vars == 2) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_optima.appendObj(-186.7309);
			}
		}
		else if (m_num_vars == 3) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_optima.appendObj(-2709.09);
			}
		}
		else if (m_num_vars == 4) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_optima.appendObj(-39303.6);
			}
		}
		m_optima.setObjectiveGiven(true);
	}

	void Shubert::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 1;
		for (int j = 0; j < m_num_vars; ++j) {
			Real a = 0;
			for (int i = 1; i <= 5; i++) {
				a += i*cos((i + 1)*x[j] + i);
			}
			s *= a;
		}
		obj[0] = s;
	}
	
}