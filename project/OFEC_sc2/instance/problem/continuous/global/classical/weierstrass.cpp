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

#include "weierstrass.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Weierstrass::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-0.5, 0.5);
		m_a = 0.5;
		m_b = 3;
		m_kmax = 20;
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_variable_niche_radius = 1e-4 * 0.5 * m_num_vars;
		m_objective_accuracy = 1e-8;
	}

	void Weierstrass::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real fit = 0, s = 0;
		for (int i = 0; i < m_num_vars; i++)
			for (int k = 0; k <= m_kmax; k++)
				fit += pow(m_a, k) * cos(2 * OFEC_PI * pow(m_b, k) * (x[i] + 0.5));

		for (int k = 0; k <= m_kmax; k++)
			s += pow(m_a, k) * cos(2 * OFEC_PI * pow(m_b, k) * 0.5);
		s = s * m_num_vars;
		obj[0] = fit - s + m_bias;
	}

}