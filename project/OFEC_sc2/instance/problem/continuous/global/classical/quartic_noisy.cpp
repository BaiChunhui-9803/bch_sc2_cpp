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

#include "quartic_noisy.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {	
	void QuarticNoisy::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-1.28, 1.28);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
		m_variable_accuracy = 1.0e-2;
	}

	void QuarticNoisy::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real fitness = 0;
		for (int i = 0; i < m_num_vars; i++) {
			fitness += (i + 1)*pow(x[i], 4);
		}
		fitness += GET_RND(m_id_rnd).uniform.next();
		obj[0] = fitness + m_bias;
	}
	
}