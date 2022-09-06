//Register dMOP1 "dMOP1" DMOP,ConOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Qingshan Tan
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

* see https://github.com/Changhe160/OFEC for more information
*************************************************************************/

/************************************************************************
Chi-Keong Goh and Kay Chen Tan (2009).
A Competitive-Cooperative Coevolutionary Paradigm for Dynamic Multiobjective Optimization
IEEE Transactions on evolutionary computation, 13(1), 103-127.
************************************************************************/

// Created: 5 August 2019 by Qingshan Tan
// Last modified at 5 August 2019 by Qingshan Tan


#ifndef dMOP1_H
#define dMOP1_H


#include "../DMOPs.h"
//#include "../../../../../core/algorithm/population.h"

namespace ofec {
	class dMOP1 final : public DMOPs{
	public:
		dMOP1(const ParamMap &v);
		dMOP1(const std::string &name, size_t size_var);
		~dMOP1() {}
		void initialize();
		void generateAdLoadPF();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif //dMOP1_H

