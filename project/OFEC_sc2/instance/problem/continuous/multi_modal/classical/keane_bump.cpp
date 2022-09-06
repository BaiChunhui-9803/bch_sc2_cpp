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
*  Paper; Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*******************************************************************************************/

#include "keane_bump.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void KeaneBump::initialize_() { 
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(1e-8, 10);
		m_objective_accuracy = 0.5;
		m_variable_accuracy = 1.e-4;
		// note: no optima
		m_optima.clear();
	}

	void KeaneBump::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s, a = 0, b = 1, c = 0;
		int i;
		for (i = 0; i < m_num_vars; i++) {
			a += pow(cos(x[i]), 4);
			b *= cos(x[i]) * cos(x[i]);
			c += (i + 1) * x[i] * x[i];
		}
		s = abs((a - 2 * b) / sqrt(c));
		obj[0] = s;
	}
	
}