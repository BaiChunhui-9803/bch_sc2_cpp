//Register HE5 "HE05" DMOP,ConOP

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
Marde Helbig and Andries P Engelbrecht.
Benchmarks for dynamic multi-objective optimisation.
In 2013 IEEE Symposium on Computational Intelligence in Dynamic and Uncertain Environments (CIDUE), pages 84-91.IEEE, 2013
************************************************************************/

// Created: 5 August 2019 by Qingshan Tan
// Last modified at 5 August 2019 by Qingshan Tan


#ifndef HE5_H
#define HE5_H


#include "../DMOPs.h"

namespace ofec {
	class HE5 final : public DMOPs {
	public:
		HE5(const ParamMap &v);
		HE5(const std::string &name, size_t size_var);
		~HE5() {}
		void initialize();
		void generateAdLoadPF();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif //HE5_H


