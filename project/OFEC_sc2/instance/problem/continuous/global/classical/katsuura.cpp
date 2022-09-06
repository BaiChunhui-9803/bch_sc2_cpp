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

#include "katsuura.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void katsuura::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-100., 100.);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void katsuura::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		size_t i,j;
		Real temp, tmp1, tmp2, tmp3;
		obj[0] = 1.0;
		tmp3 = pow(1.0*m_num_vars, 1.2);
		for (i = 0; i<m_num_vars; i++) {
			temp = 0.0;
			for (j = 1; j <= 32; ++j) {
				tmp1 = pow(2.0, j);
				tmp2 = tmp1*x[i];
				temp += fabs(tmp2 - floor(tmp2 + 0.5)) / tmp1;
			}
			obj[0] *= pow(1.0 + (i + 1)*temp, 10.0 / tmp3);
		}
		tmp1 = 10.0 / m_num_vars / m_num_vars;
		obj[0] = obj[0] * tmp1 - tmp1;
		obj[0] += m_bias;
	}
}