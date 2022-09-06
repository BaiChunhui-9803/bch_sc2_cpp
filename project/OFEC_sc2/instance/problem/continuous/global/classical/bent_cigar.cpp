
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

#include "bent_cigar.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void BentCigar::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-100., 100.);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void BentCigar::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		obj[0] = x[0] * x[0];
		for (size_t i = 1; i< m_num_vars; ++i)	{
			obj[0] += pow(10.0, 6.0) * x[i] * x[i];
		}
		obj[0] += m_bias;
	}

}