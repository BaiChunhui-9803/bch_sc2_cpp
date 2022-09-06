
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

#include "ackley.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Ackley::initialize_() {
		Function::initialize_();
		m_objective_accuracy = 1e-8;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		m_variable_niche_radius = 1e-4 * 32.768 * m_num_vars;
		setDomain(-32.768, 32.768);
		m_opt_mode[0] = OptMode::kMinimize;		
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void Ackley::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real s1 = 0, s2 = 0;
		for (int i = 0; i < m_num_vars; i++) {
			s1 += x[i] * x[i];
			s2 += cos(2 * OFEC_PI*x[i]);
		}
		obj[0] = -20 * exp(-0.2*sqrt(s1 / m_num_vars)) - exp(s2 / m_num_vars) + 20 + OFEC_E + m_bias;
	}
	
}