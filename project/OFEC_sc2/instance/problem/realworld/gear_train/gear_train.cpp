#include "gear_train.h"

namespace ofec {
	void GearTrain::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;

		resizeVariable(4);
		setDomain(12, 60);

		m_optima.clear();
		VarVec<Real> opt_var({ 15,20,57,59 });
		std::vector<Real> opt_obj(1);
		evaluateObjective(opt_var.data(), opt_obj);
		m_optima.appendVar(opt_var);
		m_optima.appendObj(opt_obj);
		m_optima.setVariableGiven(true);
		m_optima.setObjectiveGiven(true);
	}

	void GearTrain::evaluateObjective(Real *x, std::vector<Real> &obj) {
		int x1, x2, x3, x4;
		Real s;
		x1 = (int)x[0]; x2 = (int)x[1]; x3 = (int)x[2]; x4 = (int)x[3];
		s = 1. / 6.931 - x1*x2 / (Real)(x3*x4);
		obj[0] = s*s;
	}
}