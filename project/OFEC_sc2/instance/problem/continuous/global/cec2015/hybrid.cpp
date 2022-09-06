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

#include "hybrid.h"
#include "../../../../../core/instance_manager.h"
#include <numeric>

namespace ofec {
	namespace CEC2015 {
		Hybrid::~Hybrid() {
			for (auto &i : m_function)
				if (i) delete i;
			for (int id : m_param_ids)
				DEL_PARAM(id);
		}

		void Hybrid::initialize_() {
			Continuous::initialize_();
			resizeObjective(1);
			m_opt_mode[0] = OptMode::kMinimize;

			auto &v = GET_PARAM(m_id_param);
			resizeVariable(std::get<int>(v.at("number of variables")));
			setDomain(-100., 100.);
			m_random_perm.resize(m_num_vars);
			std::iota(m_random_perm.begin(), m_random_perm.end(), 0);
			GET_RND(m_id_rnd).uniform.shuffle(m_random_perm.begin(), m_random_perm.end());
			
			for (auto &i : m_function)
				if (i) delete i;
			for (int id : m_param_ids)
				DEL_PARAM(id);
			setFunction();

			// Set optimal solution
			VarVec<Real> var(m_num_vars);
			std::vector<Real> obj;
			size_t num = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				for (size_t j = 0; j < m_dim[i]; ++j)
					var[m_random_perm[num++]] = m_function[i]->getOptima().variable(0)[j];
			}
			evaluateObjective(var.data(), obj);
			m_optima.appendVar(var);
			m_optima.appendObj(obj);
			m_optima.setVariableGiven(true);
			m_optima.setObjectiveGiven(true);
		}

		void Hybrid::evaluateObjective(Real *x, std::vector<Real> &obj) {
			size_t count = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				for (size_t j = 0; j < m_dim[i]; ++j)
					m_temp_sol[i].variable()[j] = x[m_random_perm[count++]];
				//std::copy(x + m_start[i], x + m_start[i] + m_dim[i], temp_var.begin());
				m_function[i]->evaluate(m_temp_sol[i], -1, false);
				obj[0] += m_temp_sol[i].objective(0);
			}
		}		
	}
}

