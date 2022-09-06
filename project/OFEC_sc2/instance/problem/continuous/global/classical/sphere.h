//Register Sphere "Classic_sphere" GOP,ConOP,SOP

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

#ifndef OFEC_SPHERE_H
#define OFEC_SPHERE_H

#include "../../../../../core/problem/continuous/function.h"
#include "../metrics_gop.h"

namespace ofec {
	class Sphere : public Function, public MetricsGOP {
	protected:
		void initialize_() override;
		void evaluateOriginalObj(Real *x, std::vector<Real>& obj) override;
	};	
}
#endif // !OFEC_SPHERE_H
