//Register FDA3 "FDA3" DMOP,ConOP

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
Farina, M., Deb, K., & Amato, P. (2004).
Dynamic multiobjective optimization problems: test cases, approximations, and applications.
IEEE Transactions on evolutionary computation, 8(5), 425-442.
************************************************************************/

// Created: 5 August 2019 by Qingshan Tan
// Last modified at 5 August 2019 by Qingshan Tan


#ifndef FDA3_H
#define FDA3_H


#include "../DMOPs.h"

namespace ofec {
	class FDA3 final : public DMOPs {
	public:
		FDA3(const ParamMap &v);
		FDA3(const std::string &name, size_t size_var);
		~FDA3() {}
		void initialize();
		void generateAdLoadPF();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif //FDA3_H