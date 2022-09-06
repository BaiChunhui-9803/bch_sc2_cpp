//Register Penalized_1 "Classic_penalized_1" GOP,ConOP,SOP

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

#ifndef OFEC_PENALIZED_1_H
#define OFEC_PENALIZED_1_H

#include "../../../../../core/problem/continuous/function.h"
#include "../metrics_gop.h"

namespace ofec {
	class Penalized_1 : public Function, public MetricsGOP {
	protected:
		void initialize_() override;
		void evaluateOriginalObj(Real *x, std::vector<Real> &obj) override;
		Real u(Real x, Real a, Real k, Real m)const;
	}; 	
}
#endif // !OFEC_PENALIZED_1_H
