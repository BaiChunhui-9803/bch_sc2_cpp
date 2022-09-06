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

#include "scaffer_F6.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ScafferF6::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		resizeVariable(std::get<int>(GET_PARAM(m_id_param).at("number of variables")));
		m_variable_niche_radius = 1e-4 * 100 * m_num_vars;
		setDomain(-100., 100.);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_objective_accuracy = 1e-8;
	}

	void ScafferF6::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real fitness = 0;
		for (size_t i = 0; i < m_num_vars-1; ++i) {
			fitness += 0.5 + (sin(sqrt(x[i] * x[i] + x[i+1] * x[i+1]))*sin(sqrt(x[i] * x[i] + x[i+1] * x[i+1])) - 0.5) / ((1 + 0.001*(x[i] * x[i] + x[i+1] * x[i+1]))*(1 + 0.001*(x[i] * x[i] + x[i+1] * x[i+1])));
		}
		obj[0] = fitness + m_bias;
	}
	
}