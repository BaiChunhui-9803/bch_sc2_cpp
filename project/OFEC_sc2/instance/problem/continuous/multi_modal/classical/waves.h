//Register Waves "Classic_waves" MMOP,ConOP,SOP

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
******************************************************************************************
*  Paper: Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*******************************************************************************************/

#ifndef OFEC_WAVES_H
#define OFEC_WAVES_H

#include "../metrics_mmop.h"

namespace ofec {	
	class Waves : public MetricsMMOP {
	protected:
		void initialize_();
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;
	};	
}
#endif // !OFEC_WAVES_H