//Register CEC2015_GOP_F05 "GOP_CEC2015_F05" GOP,ConOP,SOP

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
#ifndef OFEC_F5_SR_SCHWEFEL_H
#define OFEC_F5_SR_SCHWEFEL_H

#include "../classical/schwefel.h"

namespace ofec {
	namespace CEC2015 {
		class F5_SR_schwefel final : public Schwefel {
		protected:
			void initialize_() override;
		};
	}
	using CEC2015_GOP_F05 = CEC2015::F5_SR_schwefel;
}

#endif // !OFEC_F5_SR_SCHWEFEL_H





