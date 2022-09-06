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

#include "waves.h"

namespace ofec {
	void Waves::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;

		resizeVariable(2);
		m_domain.setRange(-0.9, 1.2, 0);
		m_domain.setRange(-1.2, 1.2, 1);

		//one global optimum+9 local optimum
		m_optima.clear();
		Real var_data[10][3] = {
			-0.60569,-1.17756,7.307,
			1.2,1.2,7.30426,
			0.617713,0.894277,6.0734,
			0.878926,1.2,5.16511,
			0.208297,1.2,5.16617,
			1.00628,5.18024e-008,4.68647,
			-0.172694,-4.97821e-008,3.98954,
			0.586504,-0.776704,3.62485,
			-0.609362,0.807224,3.17582,
			0.161838,-1.2,2.93141 };
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		for (int i = 0; i < 10; ++i) {
			var[0] = var_data[i][0];
			var[1] = var_data[i][1];
			evaluateObjective(var.data(), obj);
			m_optima.appendVar(var);
			m_optima.appendObj(obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
		m_objective_accuracy = 0.15;
		m_variable_accuracy = 1.e-3;
		m_mb = MeasureBy::kVar;
	}

	void Waves::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s, t;
		t = x[1] * x[1];
		s = pow(0.3 * x[0], 3) + 3.5 * x[0] * pow(x[1], 3) - 4.7 * cos(3 * x[0] - (2 + x[0]) * t) * sin(2.5 * x[0] * OFEC_PI);
		obj[0] = s;
	}

}