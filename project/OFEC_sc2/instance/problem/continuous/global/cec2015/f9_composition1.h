//Register CEC2015_GOP_F09 "GOP_CEC2015_F09" GOP,ConOP,SOP

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
*************************************************************************
*  Paper : Problem Definitions and Evaluation Criteria for the CEC2015
*  Competition on Learning-based Real-Parameter Single Objective
*  Optimization.
************************************************************************/

#ifndef OFEC_F9_GLOBAL_COMPOSITION1_H
#define OFEC_F9_GLOBAL_COMPOSITION1_H

#include "composition.h"

namespace ofec {
	namespace CEC2015 {
		class F9_global_composition1 final : public composition_2015 {
		protected:
			void initialize_() override;
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction() override;
		};
	}
	using CEC2015_GOP_F09 = CEC2015::F9_global_composition1;
}
#endif // !OFEC_F9_GLOBAL_COMPOSITION1_H

