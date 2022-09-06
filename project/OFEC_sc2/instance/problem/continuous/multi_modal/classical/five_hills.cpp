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

#include "five_hills.h"

namespace ofec {
	void FiveHills::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;

		resizeVariable(2);
		m_domain.setRange(-2.5, 3., 0);
		m_domain.setRange(-2, 2., 1);

		 //1 gopt + 4 lopt
		m_optima.clear();
		Real var_data[5][3] = {
			-8.24371e-014,-1.53082e-013,2.5,
			0.889286,-1.08335e-016,1.60084,
			-0.889286,-8.32809e-017,1.60084,
			-1.78391,1.09381e-016,0.699112,
			1.78391,1.09381e-016,0.699112 };
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		for (int i = 0; i < 5; ++i) {
			var[0] = var_data[i][0];
			var[1] = var_data[i][1];
			evaluateObjective(var.data(), obj);
			m_optima.appendVar(var);
			m_optima.appendObj(obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
		m_objective_accuracy = 0.2;
		m_variable_accuracy = 1.e-5;
		m_mb = MeasureBy::kVar;
	}

	void FiveHills::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s;
		s = sin(2.2*OFEC_PI*x[0] + 0.5*OFEC_PI)*(2 - fabs(x[1])) / 2 * (3 - fabs(x[0])) / 2 + sin(0.5*OFEC_PI*x[1] + 0.5*OFEC_PI)*(2 - fabs(x[1])) / 2 * (2 - fabs(x[0])) / 2;
		obj[0] = s;
	}
	
}