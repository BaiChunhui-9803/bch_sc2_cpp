//Register CEC2013_MMOP_F12 "MMOP_CEC2013_F12" MMOP,ConOP,SOP

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
*  Paper: Benchmark Functions for CEC��2013 Special Session and Competition on Niching
*  Methods for Multimodal Function Optimization.
*******************************************************************************************/

#ifndef OFEC_CEC13_COMPOSITION4_H
#define OFEC_CEC13_COMPOSITION4_H

#include "composition.h"

namespace ofec {
	namespace cec2013 {
		class Composition4 final : public Composition {
		protected:
			void setFunction() override;
		};
	}
	using CEC2013_MMOP_F12 = cec2013::Composition4;
}
#endif // !OFEC_CEC13_COMPOSITION4_H



