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
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
// Created: 21 July 2011
// Last modified:

#include "iba.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void IBA::setPara() {
		if (m_case == 1) {
			m_kappa = -0.95;
			m_chi = -1.26;
		}
		else {
			m_kappa = 4;
			m_chi = 2;
		}
	}

	void IBA::setCase(int c) {
		m_case = c;
		setPara();
	}

	void IBA::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		resizeVariable(2);
		setDomain(-4.0, 4.0);
		m_variable_accuracy = 1.e-6;
		auto &v = GET_PARAM(m_id_param);
		m_case = v.count("case") > 0 ? std::get<int>(v.at("case")) : 1;
		setPara();
		m_optima.clear();
		VarVec<Real> opt_var(2);
		std::vector<Real> opt_obj(1);
		std::vector<std::vector<Real>> var_data;
		if (m_case == 1) {
			m_objective_accuracy = 0.08;
			var_data = { { 0.45186f, 0.0f },{ -0.22593f, 0.39132f },{ -0.22593f, -0.39132f },{ 0.0f, 0.0f } };
		}
		else {
			m_objective_accuracy = 0.5;
			var_data = { { 0.0f, 0.0f },{ 1.2243f, 0.0f },{ -0.61215f, 1.0603f },{ -0.61215f, -1.0603f } };
		}
		for (auto &i : var_data) {
			opt_var[0] = i[0];
			opt_var[1] = i[1];
			evaluateObjective(opt_var.data(), opt_obj);
			m_optima.appendVar(opt_var);
			m_optima.appendObj(opt_obj);
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
		m_mb = MeasureBy::kVar;
	}
	
	void IBA::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		Real t0 = x[0] * x[0] + x[1] * x[1];
		s = (t0) / (1 + t0) + m_kappa*(14 * t0 + pow(t0, 2)*m_chi*m_chi - 2 * sqrt(14.)*(pow(x[0], 3) - 3 * x[0] * x[1] * x[1])*m_chi) / (14 * (pow(1 + t0, 2)));
		obj[0] = s;
	}
	
}