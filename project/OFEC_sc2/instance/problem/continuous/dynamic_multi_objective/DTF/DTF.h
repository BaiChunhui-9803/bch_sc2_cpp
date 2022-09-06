//Register DTF "DTF" DMOP,ConOP

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
Jorn Mehnen, Tobias Wagner, and Gunter Rudolph. 
Evolutionary optimization of dynamic multi-objective test functions. 
In Proc. of 2nd Italian Workshop on Evolutionary Computation and 3rd Italian Workshop on Artificial Life, 2006
************************************************************************/

// Created: 5 August 2019 by Qingshan Tan
// Last modified at 5 August 2019 by Qingshan Tan

#ifndef DTF_H
#define DTF_H

#include "../DMOPs.h"
//#include "../../dynamic/dynamic.h"

namespace ofec {
	class DTF :public DMOPs {
	public:
		DTF(const ParamMap &v);
		DTF(const std::string &name, size_t size_var);
		~DTF() {};
		void initialize();
		void generateAdLoadPF();
	protected:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}
#endif //DTF_H