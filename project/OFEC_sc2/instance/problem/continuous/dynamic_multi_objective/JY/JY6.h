//Register JY6 "JY06" DMOP,ConOP

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
Shouyong Jiang and Shengxiang Yang.
Evolutionary dynamic multiobjective optimization: Benchmarks and algorithm comparisons.
IEEE transactions on cybernetics, 47(1):198-211, 2016
************************************************************************/

// Created: 5 August 2019 by Qingshan Tan
// Last modified at 5 August 2019 by Qingshan Tan


#ifndef JY6_H
#define JY6_H


#include "../DMOPs.h"

namespace ofec {
	class JY6 final : public DMOPs {
	public:
		JY6(const ParamMap &v);
		JY6(const std::string &name, size_t size_var);
		~JY6() {}
		void initialize();
		void generateAdLoadPF();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif //JY6_H

