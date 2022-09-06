/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#include "max_global.h"

namespace ofec {
	void MaxGlobal::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		resizeVariable(1);
		setDomain(0, 1);
		m_objective_accuracy = 0.1;
		m_variable_accuracy = 1.e-5;
		//5 gopt
		m_optima.clear();
		std::vector<Real> opt_x = { 0.5f, 0.1f, 0.3f, 0.7f, 0.9f };
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

	void MaxGlobal::evaluateObjective(Real *x, std::vector<Real> &obj) {
		obj[0] = pow(sin(5 * OFEC_PI*x[0]), 6.);
	}
	
}