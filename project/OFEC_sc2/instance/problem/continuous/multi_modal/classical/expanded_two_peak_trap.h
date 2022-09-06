//Register ExpandedTwoPeakTrap "Classic_two_peak_trap_expanded" MMOP,ConOP,SOP

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
*  Paper: Problem Definitions and Evaluation Criteria for the CEC 2015
*  Competition on Single Objective Multi-Niche Optimization.
*******************************************************************************************/

#ifndef OFEC_EXPANDED_TWO_PEAK_TRAP_H
#define OFEC_EXPANDED_TWO_PEAK_TRAP_H

#include "../../../../../core/problem/continuous/function.h"
#include "../metrics_mmop.h"

namespace ofec {	
	class ExpandedTwoPeakTrap : public Function, public MetricsMMOP {
	protected:
		void initialize_();
		void evaluateOriginalObj(Real *x, std::vector<Real> &obj) override;
	};	
}
#endif // !OFEC_EXPANDED_TWO_PEAK_TRAP_H
