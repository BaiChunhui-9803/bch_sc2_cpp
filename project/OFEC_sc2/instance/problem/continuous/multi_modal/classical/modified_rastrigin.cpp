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
*  Paper; A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
/*******************************************************************************************
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
****************************************************************************************/

#include "modified_rastrigin.h"

namespace ofec {
	void ModifiedRastrigin::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;

		resizeVariable(2);
		setDomain(0.0, 1.); // note
		setInitialDomain(0.0, 1.); // note
		
		m_k.resize(2);
		m_k[0] = 3; m_k[1] = 4;
		m_optima.clear();
		Real var_data[12][2] = {
			0.500342,0.625301,
			0.166184,0.375187,
			0.499985,0.375069,
			0.166625,0.625437,
			0.499419,0.875302,
			0.500066,0.124804,
			0.833502,0.874675,
			0.833978,0.374673,
			0.165937,0.124908,
			0.833364,0.624431,
			0.167073,0.875503,
			0.833226,0.124721};
		VarVec<Real> var(2);
		std::vector<Real> obj(1);
		for (int i = 0; i < 12; ++i) {
			var[0] = var_data[i][0];
			var[1] = var_data[i][1];
			evaluateObjective(var.data(), obj);
			m_optima.appendVar(var);
			m_optima.appendObj(obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
		m_objective_accuracy = 0.1;
		m_variable_accuracy = 1.e-5;
		m_mb = MeasureBy::kVar;	}

	void ModifiedRastrigin::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		for (int i = 0; i < m_num_vars; ++i) {
			s += 10 + 9 * cos(2 * OFEC_PI*m_k[i] * x[i]);
		}
		obj[0] = s;  // note
	}
	
}