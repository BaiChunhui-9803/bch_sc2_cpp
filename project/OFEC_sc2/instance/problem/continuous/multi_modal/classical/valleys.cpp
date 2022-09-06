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

#include "valleys.h"

namespace ofec {
	void Valleys::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		resizeVariable(2);
		m_domain.setRange(-2.5, 3, 0);
		m_domain.setRange(-2, 2, 1);
		m_objective_accuracy = 0.5;
		m_variable_accuracy = 1.e-4;
		 //1 gopt + 1 lopt
		m_optima.clear();
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		std::vector<std::vector<Real>> var_data = { 
			{ 1.69714f, 0.0f }, 
			{-1.44446f, 0.0f } };
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

	void Valleys::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s;
		s = sin(2 * x[0] - 0.5*OFEC_PI) + 3 * cos(x[1]) + 0.5*x[0];
		obj[0] = s;
	}
	
}