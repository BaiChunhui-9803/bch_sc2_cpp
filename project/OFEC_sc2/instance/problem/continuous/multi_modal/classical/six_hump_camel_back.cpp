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

#include "six_hump_camel_back.h"

namespace ofec {
	void SixHumpCamelBack::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		resizeVariable(2);
		m_domain.setRange(-1.9, 1.9, 0);
		m_domain.setRange(-1.1, 1.1, 1);
		m_variable_accuracy = 1.e-4;
		m_objective_accuracy = 0.1;
		// 2gopt+ 4 lopt
		m_optima.clear();
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		std::vector<std::vector<Real>> var_data = { 
			{-0.089842f, 0.712656f }, 
			{0.089842f, -0.712656f},
			{-1.70361f, 0.796084f}, 
			{1.70361f, -0.796084f}, 
			{-1.6071f,-0.56865f}, 
			{1.6071f, 0.56865f} };
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

	void SixHumpCamelBack::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = x[0] * x[0], t = x[1] * x[1];
		s = (4 - 2.1*s + pow(x[0], 4) / 3)*s + x[0] * x[1] + (-4 + 4 * t)*t;
		obj[0] = s;
	}
	
}