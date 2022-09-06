//Register UDF6 "UDF6" DMOP,ConOP

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
Subhodip Biswas, Swagatam Das, Ponnuthurai N Suganthan, and Carlos A Coello Coello.
Evolutionary multiobjective optimization in dynamic environments: A set of novel benchmark functions.
In 2014 IEEE Congress on Evolutionary Computation (CEC), pages 3192-3199. IEEE, 2014
************************************************************************/

// Created: 5 August 2019 by Qingshan Tan
// Last modified at 5 August 2019 by Qingshan Tan

#ifndef UDF6_H
#define UDF6_H


#include "../DMOPs.h"

namespace ofec {
	class UDF6 final : public DMOPs {
	public:
		UDF6(const ParamMap &v);
		UDF6(const std::string &name, size_t size_var);
		~UDF6() {}
		void initialize();
		void generateAdLoadPF();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif //UDF6_H

