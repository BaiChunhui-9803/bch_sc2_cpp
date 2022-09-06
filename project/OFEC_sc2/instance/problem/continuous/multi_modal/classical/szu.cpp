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
******************************************************************************************
*  Paper; A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
* //* 
//* F(vec3{X})=\sum_{i=1}^{D}{-x_i^2}
//*
*******************************************************************************************/

#include "szu.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Szu::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;

		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-5.0, 5.0);

		m_optima.clear();
		if (m_num_vars >= 2 && m_num_vars <= 9) {
			switch (m_num_vars) {
				case 2:	m_optima.appendObj(-156.66); break;
				case 3:	m_optima.appendObj(-235.0); break;
				case 4:	m_optima.appendObj(-313.33); break;
				case 5:	m_optima.appendObj(-391.66); break;
				case 6:	m_optima.appendObj(-469.99); break;
				case 7:	m_optima.appendObj(-548.33); break;
				case 8:	m_optima.appendObj(-626.66); break;
				case 9:	m_optima.appendObj(-704.99); break;
			}
			m_optima.setObjectiveGiven(true);
		}
		//m_objective_accuracy = 0.1;
	}

	void Szu::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		size_t i;
		for (i = 0; i < m_num_vars; ++i) {
			s += pow(x[i], 4) - 16 * x[i] * x[i] + 5 * x[i];
		}
		obj[0] = s;
	}
	
}