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

#include "center_peak.h"

namespace ofec {
	void CenterPeak::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		resizeVariable(2);
		setDomain(-2, 2);
		m_objective_accuracy = 1.e-5;
		m_variable_accuracy = 0.2;
		m_optima.clear();
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		std::vector<std::vector<Real>> var_data = { {-6.02513e-012f, -2.41155e-012f}, {2, -2}, {-2, 2}, {-2, -2}, {2, 2} };
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

	void CenterPeak::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s;
		s = 3 * sin(0.5*OFEC_PI*x[0] + 0.5*OFEC_PI)*(2 - sqrt(x[0] * x[0] + x[1] * x[1])) / 4;
		obj[0] = s;
	}	
}