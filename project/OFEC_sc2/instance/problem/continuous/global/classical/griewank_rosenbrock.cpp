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

#include "griewank_rosenbrock.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void GriewankRosenbrock::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		m_variable_niche_radius = 1e-4 * 5 * m_num_vars;
		setDomain(-5, 5);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_objective_accuracy = 1e-8;
	}

	void GriewankRosenbrock::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real result = 0;
		for (size_t i = 0; i < m_num_vars; ++i) {
			Real result_f2 = 0;
			Real result_f8 = 0;
			Real x_front = x[i] + 1;
			Real x_back = x[(i + 1) % m_num_vars] + 1;

			result_f2 += 100 * pow((x_back - x_front * x_front), 2.0) + (x_front - 1) * (x_front - 1);
			result_f8 += result_f2 * result_f2 / 4000.0 - cos(result_f2 / sqrt((Real)(i + 1))) + 1;
			result += result_f8;
		}
		result += m_bias;
		obj[0] = result;
	}
	
}