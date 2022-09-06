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

#include "schwefel.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Schwefel::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-500, 500);
		std::vector<Real> var(m_num_vars, 420.9687);
		setOriginalGlobalOpt(var.data());
		m_optima = m_original_optima;
		m_variable_accuracy = 1.0e-2;
	}

	void Schwefel::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real fitness = 0;
		for (int i = 0; i < m_num_vars; i++) {
			fitness += -x[i] * sin(sqrt(fabs(x[i])));
		}
		obj[0] = fitness + m_bias;
	}	
}