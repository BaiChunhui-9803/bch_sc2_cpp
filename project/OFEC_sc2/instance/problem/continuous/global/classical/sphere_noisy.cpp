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

#include "sphere_noisy.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void SphereNoisy::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		m_variable_niche_radius = 1e-4 * 100 * m_num_vars;
		setDomain(-100., 100.);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_objective_accuracy = 1e-8;
	}

	void SphereNoisy::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real fit = 0;
		Real noise;
		for (int i = 0; i < m_num_vars; i++) {
			noise = 0.01 * GET_RND(m_id_rnd).uniform.next();
			fit += (x[i] + noise)*(x[i] + noise);
		}
		obj[0] = fit + m_bias;
	}
	
}