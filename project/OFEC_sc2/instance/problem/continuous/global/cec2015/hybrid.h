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

#ifndef OFEC_CEC2015_HYBRID_H
#define OFEC_CEC2015_HYBRID_H

#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/problem/continuous/function.h"
#include "../../../../../core/problem/solution.h"

namespace ofec {
	namespace CEC2015 {
		class Hybrid : public Continuous {
		public:
			~Hybrid();
			size_t get_num_function() { return m_num_function; }
			Function* get_function(size_t num) { return m_function[num]; }
			std::vector<size_t>& dim() { return m_dim; }
			std::vector<size_t>& random_perm() { return m_random_perm; }
			std::vector<Real>& hybrid_translation() { return m_hybrid_translation; }

		protected:
			void initialize_() override;
			void evaluateObjective(Real *x, std::vector<Real> &obj) override;
			virtual void setFunction() = 0;

		protected:
			size_t m_num_function;
			std::vector<Function*> m_function;    // the functions
			std::vector<int> m_param_ids;		  // param id of functions
			std::vector<size_t> m_start, m_dim;
			std::vector<size_t> m_random_perm;
			std::vector<Real> m_hybrid_translation;
			std::vector<Solution<>> m_temp_sol;
		};

	}
}
#endif // ! OFEC_CEC2015_HYBRID_H

