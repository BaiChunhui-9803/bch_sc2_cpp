//Register IBA "Classic_IBA" MMOP,ConOP,SOP

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
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/

#ifndef OFEC_FIBA_H
#define OFEC_FIBA_H

#include "../metrics_mmop.h"

namespace ofec {
	class IBA : public MetricsMMOP {
	protected:
		void initialize_() override;
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
		void setPara();

		Real m_kappa, m_chi;
		int m_case;

	public:
		void setCase(int c);
	};
	
}
#endif // !OFEC_FIBA_H