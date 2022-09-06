//Register EqualMaxima "Classic_equal_maxima" MMOP,ConOP,SOP

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
******************************************************************************************
*  Paper: Benchmark Functions for CEC��2013 Special Session and Competition on Niching
*  Methods for Multimodal Function Optimization.
*******************************************************************************************/

#ifndef OFEC_EQUAL_MAXIMA_H
#define OFEC_EQUAL_MAXIMA_H

#include "../metrics_mmop.h"

namespace ofec {
	class EqualMaxima : public MetricsMMOP {
	protected:
		void initialize_() override;
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	};	
}

#endif // !OFEC_EQUAL_MAXIMA_H