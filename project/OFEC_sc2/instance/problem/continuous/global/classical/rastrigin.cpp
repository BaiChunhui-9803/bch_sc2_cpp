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

#include "rastrigin.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Rastrigin::initialize_() {
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-5.12, 5.12);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_variable_niche_radius = 1e-4 * 5.12 * m_num_vars;
		m_opt_mode[0] = OptMode::kMinimize;
		m_objective_accuracy = 1e-8;
	}

	void Rastrigin::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real fit = 0;
		for (int i = 0; i < m_num_vars; i++)
			fit += x[i] * x[i] - 10.*cos(2 * OFEC_PI*x[i]) + 10.;
		obj[0] = fit + m_bias;
	}
	
}