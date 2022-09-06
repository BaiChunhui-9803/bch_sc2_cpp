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

#include "dynde_individual.h"
#include "../../../../core/problem/continuous/continuous.h"
#include <cmath>

namespace ofec {
	int IndDynDE::brownian(const SolType &best, Real sigma, int id_pro, int id_alg, int id_rnd) {
		for (size_t i = 0; i < m_var.size(); ++i) {
			m_var[i] = (best.variable()[i]) + GET_RND(id_rnd).normal.nextNonStd(0, sigma);
		}
		GET_CONOP(id_pro).validateSolution(*this, Validation::kSetToBound, id_rnd);
		return evaluate(id_pro, id_alg);
	}

	int IndDynDE::quantum(const SolType &best, Real rcloud, int id_pro, int id_alg, int id_rnd) {
		const auto dim = best.variable().size();
		std::vector<Real> x(dim, 0);
		Real dis = 0;
		for (size_t i = 0; i < dim; ++i) {
			x[i] = GET_RND(id_rnd).normal.next();
			dis += x[i] * x[i];
		}
		dis = sqrt(dis);
		const auto r = GET_RND(id_rnd).uniform.nextNonStd(0.0, rcloud);
		for (size_t i = 0; i < dim; ++i) {
			m_var[i] = (best.variable()[i]) + r * x[i] / dis;
		}
		x.clear();
		GET_CONOP(id_pro).validateSolution(*this, Validation::kSetToBound, id_rnd);
		return evaluate(id_pro, id_alg);
	}

	int IndDynDE::entropy(Real sigma, int id_pro, int id_alg, int id_rnd) {
		for (auto& i : m_var)
			i = i + GET_RND(id_rnd).normal.nextNonStd(0, sigma);
		GET_CONOP(id_pro).validateSolution(*this, Validation::kSetToBound, id_rnd);
		return evaluate(id_pro, id_alg);
	}
}