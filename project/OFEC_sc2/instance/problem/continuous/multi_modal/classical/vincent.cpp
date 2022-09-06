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
*  Paper; A sequential niching memetic algorithm for Continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
/*******************************************************************************************
*  Paper: A sequential niching memetic algorithm for Continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
****************************************************************************************
*  LaTex:F(x)=\frac{1}{D}\sum^D_{i=1}{sin(10\log(x_i))}
*******************************************************************************************/

#include "vincent.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void Vincent::initialize_() { // note
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(0.25, 10.); // note
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (ofec::Real)1.e-4;
		m_variable_niche_radius = 0.2;

		m_optima.clear();
		size_t num_solution = pow(6, m_num_vars);
		std::vector<std::vector<Real>> obj_data((int)num_solution, std::vector<Real>(m_num_objs, 1));
		for (auto &i : obj_data)
			m_optima.appendObj(i);
		m_optima.setObjectiveGiven(true);

		std::vector<size_t> divs = { 0 };
		std::vector<Real> opt_x(6);
		for (int i = 0; i < 6; i++)
			opt_x[i] = exp((4 * i - 7) * OFEC_PI / 20);
		VarVec<Real> opt_var(m_num_vars);
		size_t cur_dim = 0;
		while (true) {
			if (divs[cur_dim] < 6)
				opt_var[cur_dim] = opt_x[divs[cur_dim]];
			if (divs.back() == 6) {
				divs.pop_back();
				cur_dim--;
				if (divs.empty())
					break;
				divs.back()++;
			}
			else {
				if (divs.size() == m_num_vars) {
					m_optima.appendVar(opt_var);
					divs.back()++;
				}
				else {
					divs.push_back(0);
					cur_dim++;
				}
			}
		}
		m_optima.setVariableGiven(true);
	}

	void Vincent::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		for (int i = 0; i < m_num_vars; ++i) {
			s += sin(10 * log(x[i]));
		}
		s /= m_num_vars;
		obj[0] = s;  // note
	}
}