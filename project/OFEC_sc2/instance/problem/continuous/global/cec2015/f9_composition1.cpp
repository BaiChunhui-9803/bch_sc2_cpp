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

#include "F9_global_composition1.h"
#include "../../global/classical/schwefel.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/HGBat.h"
#include "../../../../../core/problem/solution.h"

namespace ofec::CEC2015 {
	void F9_global_composition1::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;

		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-5., 5.);

		for (auto &i : m_function)
			if (i) delete i;
		for (auto id : m_param_ids)
			DEL_PARAM(id);
		m_height_normalize_severity = 2000;
		setFunction();
		loadTranslation("/instance/problem/continuous/global/CEC2015/data/");  //data path
		loadRotation("/instance/problem/continuous/global/CEC2015/data/");
		for (auto &i : m_function) {
			i->setGlobalOpt(i->translation().data());
		}

		// Set optimal solution
		Solution<> s(m_num_objs, m_num_cons, m_num_vars);
		s.variable() = m_function[0]->getOptima().variable(0);
		m_optima.appendVar(s.variable());
		evaluate(s, false);
		m_optima.appendObj(s.objective());
	}

	void F9_global_composition1::setFunction() {
		m_num_function = 3;
		m_function.resize(m_num_function);
		m_height.resize(m_num_function);
		m_converge_severity.resize(m_num_function);
		m_f_bias.resize(m_num_function);

		basic_func f(3);
		f[0] = &create_function<Schwefel>;
		f[1] = &create_function<Rastrigin>;
		f[2] = &create_function<HGBat>;

		for (size_t i = 0; i < m_num_function; ++i) {
			m_function[i] = dynamic_cast<Function*>(f[i]());
			m_function[i]->initialize();
			m_function[i]->setBias(0);
		}
		m_function[0]->setRotated(false);

		for (auto &i : m_function)
			i->setConditionNumber(2.);

		m_converge_severity[0] = 20;
		m_converge_severity[1] = 20;
		m_converge_severity[2] = 20;

		m_height[0] = 1;
		m_height[1] = 1;
		m_height[2] = 1;

		m_f_bias[0] = 0;
		m_f_bias[1] = 100;
		m_f_bias[2] = 200;
	}

	void F9_global_composition1::evaluateObjective(Real *x, std::vector<Real> &obj) {
		composition_2015::evaluateObjective(x, obj);
		obj[0] += 900;
	}
}
