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
*************************************************************************/

#include "griewank.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Griewank::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		m_variable_niche_radius = 1e-4 * 600 * m_num_vars;
		setDomain(-600, 600);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_objective_accuracy = 1e-8;
	}

	void Griewank::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real result = 0;
		Real s1 = 0, s2 = 1;
		for (int i = 0; i < m_num_vars; i++) {
			s1 += x[i] * x[i] / 4000.;
			s2 *= cos(x[i] / sqrt((Real)(i + 1)));
		}
		result = s1 - s2 + 1. + m_bias;
		obj[0] = result;
	}
	
}