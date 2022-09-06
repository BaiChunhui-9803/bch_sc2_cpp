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
#include "../../../../../core/problem/solution.h"

namespace ofec::CEC2015 {
	void composition_2015::set_weight(std::vector<Real>& weight, const std::vector<Real>&x) {
		for (size_t i = 0; i < m_num_function; ++i) { // calculate weight for each function
			weight[i] = 0;
			for (size_t j = 0; j < m_num_vars; ++j) {
				//weight[i] += pow(x[j] - m_function[i]->translation()[j], 2);
				weight[i] += pow(x[j] - m_function[i]->getOptima().variable(0)[j], 2);
			}

			if (fabs(weight[i])>1e-6) weight[i] = exp(-weight[i] / (2 * m_num_vars*m_converge_severity[i] * m_converge_severity[i])) / (Real)(pow(weight[i], 0.5));
			else {
				for (auto &m : weight) {
					m = 0;
				}
				weight[i] = 1;
				break;
			}
		}
	}

	void composition_2015::evaluateObjective(Real *x, std::vector<Real> &obj) {
		std::vector<Real> x_(m_num_vars);
		std::copy(x, x + m_num_vars, x_.begin());
		std::vector<Real> weight(m_num_function, 0);

		set_weight(weight, x_);
		std::vector<Real> fit(m_num_function);
		Solution<> s(m_num_objs, m_num_cons, m_num_vars);

		for (size_t i = 0; i < m_num_function; ++i) { // calculate objective value for each function
			s.variable().vect() = x_;
			m_function[i]->evaluate(s, false);
			fit[i] = s.objective()[0];

		}
		Real sumw = 0;
		for (size_t i = 0; i < m_num_function; ++i)
			sumw += weight[i];
		for (size_t i = 0; i < m_num_function; ++i)
			weight[i] /= sumw;

		Real temp = 0;
		for (size_t i = 0; i < m_num_function; ++i) {
			temp += weight[i] * (m_height[i] * fit[i] + m_f_bias[i]);
		}
		obj[0] = temp;
	}
}

