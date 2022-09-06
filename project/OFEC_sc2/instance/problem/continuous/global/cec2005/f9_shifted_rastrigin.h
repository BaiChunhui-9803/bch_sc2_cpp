//Register CEC2005_GOP_F09 "GOP_CEC2005_F09" GOP,ConOP,SOP

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


#ifndef OFEC_F9_SHIFTED_RASTRIGIN_H
#define OFEC_F9_SHIFTED_RASTRIGIN_H

#include "../classical/rastrigin.h"

namespace ofec {
	namespace cec2005 {
		class ShiftedRastrigin final : public Rastrigin {
		protected:
			void initialize_() override;
		};
	}
	using CEC2005_GOP_F09 = cec2005::ShiftedRastrigin;
}
#endif // ! OFEC_F9_SHIFTED_RASTRIGIN_H


