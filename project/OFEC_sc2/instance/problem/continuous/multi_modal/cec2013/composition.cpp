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

#include "composition.h"
#include "../../../../../core/instance_manager.h"

namespace ofec::cec2013 {
	void Composition::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;

		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-5., 5.);

		for (auto &i : m_function)
			if (i) delete i;
		for (auto id : m_param_ids)
			DEL_PARAM(id);
		m_height_normalize_severity = 2000;
		setFunction();
		loadRotation("/instance/problem/continuous/multi_modal/cec2013/data/");
		computeFmax();
		loadTranslation("/instance/problem/continuous/multi_modal/cec2013/data/");  //data path

		m_optima.clear();
		std::vector<Real> temp_obj(1);
		for (auto &i : m_function) {
			evaluateObjective(i->translation().data(), temp_obj);
			m_optima.appendVar(VarVec<Real>(i->translation()));
			m_optima.appendObj(temp_obj);
		}
		m_optima.setVariableGiven(true);
		m_optima.setObjectiveGiven(true);
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (ofec::Real)1.e-4;
		m_variable_niche_radius = 0.01;
	}

	void Composition::evaluateObjective(Real *x, std::vector<Real> &obj) {
		cec2005::Composition::evaluateObjective(x, obj);
		obj[0] = -obj[0];
	}
}


