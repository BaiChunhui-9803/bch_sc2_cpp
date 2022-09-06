/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#include "modified_schwefel.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ModifiedSchwefel::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-100., 100.);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void ModifiedSchwefel::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		Real tmp;
		obj[0] = 0.0;
		for (size_t i = 0; i < m_num_vars; i++){
			x[i] += 4.209687462275036e+002;
			if (x[i]>500) {
				obj[0] -= (500.0 - fmod(x[i], 500))*sin(pow(500.0 - fmod(x[i], 500), 0.5));
				tmp = (x[i] + 500.0) / 100;
				obj[0] += tmp*tmp / m_num_vars;
			}
			else if (x[i]<-500)	{
				obj[0] -= (-500.0 + fmod(fabs(x[i]), 500))*sin(pow(500.0 - fmod(fabs(x[i]), 500), 0.5));
				tmp = (x[i] + 500.0) / 100;
				obj[0] += tmp*tmp / m_num_vars;
			}
			else
				obj[0] -= x[i] * sin(pow(fabs(x[i]), 0.5));
		}
		obj[0] += 4.189828872724338e+002*m_num_vars;
		obj[0] += m_bias;
	}
}