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
*******************************************************************************************/

#include "modified_shekel.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ModifiedShekel::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;

		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		if (m_num_vars > 5) 
			throw MyExcept("Number of variables of the modified Shekel function must be <= 5@");
		setDomain(0.0, 11.0);
		setInitialDomain(0.0, 11.0);

		 //1 gopt+7 lopt
		m_optima.clear();
		Real a[8][5] = {
			4,4,6.3,4,4,1,1,8.5,
			1,1,6,6,9.1,6,6,
			3.5,7.5,4,9,4,
			5,5,3,3,9,
			9.1,8.2,2,3,9,
			1.5,9.3,7.4,3,9,
			7.8,2.2,5.3,9,3 };
		Real c[8] = { 0.1,0.2,0.4,0.15,0.6,0.2,0.06,0.18 };
		std::copy(c, c + 8, m_c);
		for (size_t i = 0; i < 8; ++i)
			std::copy(a[i], a[i] + 5, m_a[i]);
		VarVec<Real> var(m_num_vars);
		std::vector<Real> obj(1);
		for (size_t j = 0; j < m_num_vars; j++)
			var[j] = m_a[6][j];
		evaluateObjective(var.data(), obj);
		m_optima.appendVar(var);
		m_optima.appendObj(obj);
		for (size_t i = 0; i < 8; ++i) {
			if (i == 6) continue;
			for (size_t j = 0; j < m_num_vars; j++)
				var[j] = m_a[i][j];
			evaluateObjective(var.data(), obj);
			m_optima.appendVar(var);
			m_optima.appendObj(obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
		m_objective_accuracy = 0.2;
		m_variable_accuracy = 1.e-3;
		m_mb = MeasureBy::kVar;
	}

	void ModifiedShekel::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		for (int i = 0; i < 8; ++i) {
			Real b = 0;
			for (int j = 0; j < m_num_vars; ++j) {
				b += (x[j] - m_a[i][j])*(x[j] - m_a[i][j]);
			}
			s += pow(b + m_c[i], -1);
		}
		obj[0] = s;
	}
	
}