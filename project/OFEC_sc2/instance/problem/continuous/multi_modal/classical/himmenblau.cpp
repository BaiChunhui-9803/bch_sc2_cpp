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
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/

#include "himmenblau.h"

namespace ofec {
	void Himmenblau::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		resizeVariable(2);
		setDomain(-6, 6);
		m_objective_accuracy = 0.5;
		m_variable_accuracy = 1.e-4;
		// 1 gopt+3 lopt
		m_optima.clear();
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		std::vector<std::vector<Real>> var_data = {
			{ 3.0f, 2.0f },
			{ 3.58149f, -1.8208f },
			{ -2.78706f, 3.1282f },
			{ -3.76343f, -3.26605f } };
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

	void Himmenblau::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		Real t0 = (x[0] * x[0] + x[1] - 11), t1 = (x[1] * x[1] + x[0] - 7);
		s = t0 * t0 + t1 * t1 + 0.1 * (pow(x[0] - 3, 2) + pow(x[1] - 2, 2));
		obj[0] = s;
	}

}