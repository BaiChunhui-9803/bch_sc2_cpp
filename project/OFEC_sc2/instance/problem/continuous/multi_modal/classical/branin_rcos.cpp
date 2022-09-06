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
******************************************************************************************
*  Paper: Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*******************************************************************************************/

#include "branin_rcos.h"

namespace ofec {
	void BraninRCOS::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		resizeVariable(2);
		m_domain.setRange(-5, 10, 0);
		m_domain.setRange(0, 15, 1);		
		m_variable_accuracy = 0.1;
		m_objective_accuracy = 1.e-5;
		m_optima.clear();
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		std::vector<std::vector<Real>> var_data = { { -(Real)OFEC_PI, 12.275f },{ (Real)OFEC_PI, 2.275f },{9.42478f, 2.475f} };
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

	void BraninRCOS::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s, a;
		a = x[1] - 5.1*x[0] * x[0] / (4 * OFEC_PI*OFEC_PI) + 5 * x[0] / OFEC_PI - 6;
		s = a*a + 10 * (1 - 1 / (8 * OFEC_PI))*cos(x[0]) + 10;
		obj[0] = s;
	}
	
}