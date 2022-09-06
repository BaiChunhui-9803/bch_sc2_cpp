/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#include "happy_cat.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void HappyCat::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-2, 2);
		setInitialDomain(-2, 2);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void HappyCat::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real alpha, r2, sum_x;
		alpha = 1.0 / 8.0;
		r2 = 0.0;
		sum_x = 0.0;
		for (size_t i = 0; i < m_num_vars; ++i) {
			x[i] = x[i] - 1.0;  //shift to orgin
			r2 += x[i] * x[i];
			sum_x += x[i];
		}
		obj[0] = pow(fabs(r2 - m_num_vars), 2 * alpha) + (0.5*r2 + sum_x) / m_num_vars + 0.5;
		obj[0] += m_bias;
	}
}