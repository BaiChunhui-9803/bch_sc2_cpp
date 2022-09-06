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

#include "penalized_2.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Penalized_2::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-50, 50);
		std::vector<Real> var(m_num_vars, 1);
		setOriginalGlobalOpt(var.data());
		m_optima = m_original_optima;
	}

	void Penalized_2::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		for (int i = 0; i < m_num_vars - 1; i++)
			s += (x[i] - 1)*(x[i] - 1)*(1 + sin(3 * OFEC_PI*x[i + 1])*sin(3 * OFEC_PI*x[i + 1]));
		s += (x[m_num_vars - 1] - 1)*(x[m_num_vars - 1] - 1)*(1 + sin(2 * OFEC_PI*x[m_num_vars - 1])*sin(2 * OFEC_PI*x[m_num_vars - 1])) + sin(3 * OFEC_PI*x[0])*sin(3 * OFEC_PI*x[0]);
		s = s*0.1;
		for (int i = 0; i < m_num_vars; i++)
			s += u(x[i], 5, 100, 4);
		obj[0] = s + m_bias;
	}

	Real Penalized_2::u(Real x, Real a, Real k, Real m)const {
		if (x > a) return k*pow(x - a, m);
		else if (x < -a) return k*pow(-x - a, m);
		else return 0;
	}
}
