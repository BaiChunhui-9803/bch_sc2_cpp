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

#include "non_continuous_rastrigin.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void NonContinuousRastrigin::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		m_variable_niche_radius = 1e-4 * 5.12 * m_num_vars;
		setDomain(-5.12, 5.12);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_objective_accuracy = 1e-8;
	}

	void NonContinuousRastrigin::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real fit = 0;
		std::vector<Real> y(m_num_vars);
		for (size_t i = 0; i < m_num_vars; ++i) {
			if (fabs(x[i]) < 0.5) y[i] = x[i];
			else {
				Real a, b = 2 * x[i];
				if (b <= 0 && b - (int)b < 0.5) a = (int)b;
				else if (b - (int)b < 0.5) a = (int)b;
				else a = (int)b + 1;
				y[i] = a / 2;
			}
		}
		for (size_t i = 0; i < m_num_vars; ++i) {
			fit = fit + y[i] * y[i] - 10.*cos(2 * OFEC_PI*y[i]) + 10.;
		}
		obj[0] = fit + m_bias;
	}
	
}
