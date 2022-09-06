//Register ZJZ "ZJZ" DMOP,ConOP

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
Aimin Zhou, Yaochu Jin, Qingfu Zhang et.al (2009).
Predictionbased population re-initialization for evolutionary dynamic multi-objective optimization
In International Conference on Evolutionary Multi-Criterion Optimization, pages 832-846. Springer, 2007
************************************************************************/

// Created: 5 August 2019 by Qingshan Tan
// Last modified at 5 August 2019 by Qingshan Tan

#ifndef ZJZ_H
#define ZJZ_H

#include "../DMOPs.h"
//#include "../../dynamic/dynamic.h"

namespace ofec {
	class ZJZ :public DMOPs {
	public:
		ZJZ(const ParamMap &v);
		ZJZ(const std::string &name, size_t size_var);
		~ZJZ() {};
		void initialize();
		void generateAdLoadPF();
	protected:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}
#endif //ZJZ_H