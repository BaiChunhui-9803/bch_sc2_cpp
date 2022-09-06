//Register CEC2005_GOP_F07 "GOP_CEC2005_F07" GOP,ConOP,SOP

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
*************************************************************************/


#ifndef OFEC_F7_SHIFTED_ROTATED_GRIEWANK_NO_BOUND_H
#define OFEC_F7_SHIFTED_ROTATED_GRIEWANK_NO_BOUND_H

#include "../classical/griewank.h"

namespace ofec {
	namespace cec2005 {
		class ShiftedRotatedGriewankNoBound final : public Griewank {
		protected:
			void initialize_() override;
		};
	}
	using CEC2005_GOP_F07 = cec2005::ShiftedRotatedGriewankNoBound;
}
#endif // ! OFEC_F7_SHIFTED_ROTATED_GRIEWANK_NO_BOUND_H


