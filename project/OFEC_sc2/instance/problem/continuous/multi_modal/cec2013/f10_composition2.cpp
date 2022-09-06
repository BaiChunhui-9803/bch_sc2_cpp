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

#include "f10_composition2.h"
#include "../../global/classical/griewank.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/sphere.h"
#include "../../global/classical/rastrigin.h"
#include "../../../../../core/instance_manager.h"

namespace ofec::cec2013 {
	void Composition2::setFunction() {
		m_num_function = 8;
		m_function.resize(m_num_function);
		m_param_ids.resize(m_num_function);
		m_height.resize(m_num_function);
		m_fmax.resize(m_num_function);
		m_converge_severity.resize(m_num_function);
		m_stretch_severity.resize(m_num_function);

		basic_func f(4);
		f[0] = &create_function<Rastrigin>;
		f[1] = &create_function<Weierstrass>;
		f[2] = &create_function<Griewank>;
		f[3] = &create_function<Sphere>;

		for (size_t i = 0; i < m_num_function; ++i) {
			auto param = GET_PARAM(m_id_param);
			param["problem name"] = m_name + "_part" + std::to_string(i + 1);
			m_param_ids[i] = ADD_PARAM(param);
			m_function[i] = dynamic_cast<Function *>(f[i / 2]());
			m_function[i]->setIdRnd(m_id_rnd);
			m_function[i]->setIdParam(m_param_ids[i]);
			m_function[i]->initialize();
			m_function[i]->setBias(0);
		}

		m_stretch_severity[0] = 1;				m_stretch_severity[1] = 1;
		m_stretch_severity[2] = 10;				 m_stretch_severity[3] = 10;
		m_stretch_severity[4] = 1. / 10;			 m_stretch_severity[5] = 1. / 10;
		m_stretch_severity[6] = 1 / 7.;			m_stretch_severity[7] = 1 / 7.;

		for (int i = 0; i<m_num_function; i++) {
			m_function[i]->setScale(m_stretch_severity[i]);
		}

		for (int i = 0; i<m_num_function; i++) {
			m_converge_severity[i] = 1;
		}

		for (int i = 0; i<m_num_function; i++) {
			m_height[i] = 0;
		}
	}

	void Composition2::setRotation() {
		for (auto i : m_function)
			i->rotation().identify();
	}
}


