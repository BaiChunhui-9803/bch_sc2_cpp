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

#include "schwefel_1_2.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Schwefel_1_2::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		resizeVariable(std::get<int>(GET_PARAM(m_id_param).at("number of variables")));
		setDomain(-100, 100);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_variable_niche_radius = 1e-4 * 100 * m_num_vars;
		m_objective_accuracy = 1e-8;
	}

	void Schwefel_1_2::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real s1 = 0, s2 = 0;
		for (int i = 0; i < m_num_vars; i++) {
			for (int j = 0; j <= i; j++)
				s1 += x[j];
			s2 += s1*s1;
			s1 = 0;
		}
		obj[0] = s2 + m_bias;
	}
}