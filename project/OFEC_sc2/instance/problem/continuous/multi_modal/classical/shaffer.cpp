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

#include "shaffer.h"
namespace ofec {
	void Shaffer::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMaximize;
		resizeVariable(2);
		setDomain(-15, 15);
		setInitialDomain(-15, 15);
		m_variable_accuracy = 0.1;
		m_objective_accuracy = 1.e-6;
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void Shaffer::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real s, t = x[0] * x[0] + x[1] * x[1];
		s = 0.5 + (0.5 - pow(sin(sqrt(0.0001 + t)), 2)) / pow(1 + 0.001*t*t, 2);
		obj[0] = s + m_bias;
	}
	
}