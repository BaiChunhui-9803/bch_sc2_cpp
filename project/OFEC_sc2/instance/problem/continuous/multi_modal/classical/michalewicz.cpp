#include "michalewicz.h"

namespace ofec {
	void Michalewicz::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		resizeVariable(2);
		setDomain(0, OFEC_PI);
		m_m = 20;
		m_variable_accuracy = 1.e-3;
		m_objective_accuracy = 0.2;
		m_optima.clear();
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		std::vector<std::vector<Real>> var_data = { { 2.20291f, 1.5708f },{ 2.20291f, 2.71157f } };
		for (auto &i : var_data) {
			var[0] = i[0];
			var[1] = i[1];
			evaluateObjective(var.data(), obj);
			m_optima.appendVar(var);
			m_optima.appendObj(obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
	}

	void Michalewicz::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		for (int i = 0; i < m_num_vars; ++i) {
			s += sin(x[i])*pow(sin((i + 1)*x[i] * x[i] / OFEC_PI), m_m);
		}
		obj[0] = s;
	}
	
}