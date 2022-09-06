//Register SixHumpCamelBack "Classic_six_hump_camel_back" MMOP,ConOP,SOP

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
//Stoean, C.; Preuss, M.; Stoean, R.; Dumitrescu, D., 
// "Multimodal Optimization by Means of a Topological Species Conservation Algorithm," 
// Evolutionary Computation, IEEE Transactions on , vol.14, no.6, pp.842,864, Dec. 2010
//doi: 10.1109/TEVC.2010.204166
#ifndef OFEC_SIX_HUMP_H
#define OFEC_SIX_HUMP_H

#include "../metrics_mmop.h"

namespace ofec {
	class SixHumpCamelBack : public MetricsMMOP {
	protected:
		void initialize_();
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;
	};
}
#endif // !OFEC_SIX_HUMP_H