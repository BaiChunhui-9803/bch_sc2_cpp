#include "uneven_de_maxima.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void UnevenDeMaxima::initialize_() { 
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		resizeVariable(1);
		setDomain(0, 1.); // note
		m_objective_accuracy = 1.e-4;
		m_variable_niche_radius = 0.01;
		m_optima.clear();
		std::vector<Real> opt_x = { 0.08 };
		for (Real x : opt_x) {
			VarVec<Real> opt_var(1);
			std::vector<Real> opt_obj(1);
			opt_var[0] = x;
			evaluateObjective(opt_var.data(), opt_obj);
			m_optima.appendVar(opt_var);
			m_optima.appendObj(opt_obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
	}

	void UnevenDeMaxima::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real tmp1 = -2 * log(2)*((x[0] - 0.08) / 0.854)*((x[0] - 0.08) / 0.854);
		Real tmp2 = sin(5 * OFEC_PI*(pow(x[0], 3.0 / 4.0) - 0.05));
		Real s = exp(tmp1) * pow(tmp2, 6);
		obj[0] = s;  // note
	}
}