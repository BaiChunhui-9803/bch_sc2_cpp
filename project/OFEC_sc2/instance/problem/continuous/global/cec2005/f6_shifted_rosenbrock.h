//Register CEC2005_GOP_F06 "GOP_CEC2005_F06" GOP,ConOP,SOP

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


#ifndef OFEC_F6_SHIFTED_ROSENBROCK_H
#define OFEC_F6_SHIFTED_ROSENBROCK_H

#include "../classical/rosenbrock.h"

namespace ofec {
	namespace cec2005 {
		class ShiftedRosenbrock final : public Rosenbrock {
		protected:
			void initialize_() override;
		};
	}
	using CEC2005_GOP_F06 = cec2005::ShiftedRosenbrock;
}
#endif // ! OFEC_F6_SHIFTED_ROSENBROCK_H

