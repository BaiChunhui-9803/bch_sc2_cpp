//Register CEC2005_GOP_F02 "GOP_CEC2005_F02" GOP,ConOP,SOP

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


#ifndef OFEC_F2_SHIFTED_SCHWEFEL_1_2_H
#define OFEC_F2_SHIFTED_SCHWEFEL_1_2_H

#include "../classical/schwefel_1_2.h"

namespace ofec {
	namespace cec2005 {
		class ShiftedSchwefel_1_2 final : public Schwefel_1_2 {
		protected:
			void initialize_() override;
		};
	}
	using CEC2005_GOP_F02 = cec2005::ShiftedSchwefel_1_2;
}
#endif // ! OFEC_F2_SHIFTED_SCHWEFEL_1_2_H
