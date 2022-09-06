//Register CEC2005_GOP_F11 "GOP_CEC2005_F11" GOP,ConOP,SOP

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


#ifndef OFEC_F11_SHIFTED_ROTATED_WEIERSTRASS_H
#define OFEC_F11_SHIFTED_ROTATED_WEIERSTRASS_H

#include "../classical/weierstrass.h"
#include "../metrics_gop.h"

namespace ofec {
	namespace cec2005 {
		class ShiftedRotatedWeierstrass final: public Weierstrass {
		protected:
			void initialize_() override;
		};
	}
	using CEC2005_GOP_F11 = cec2005::ShiftedRotatedWeierstrass;
}
#endif // ! OFEC_F11_SHIFTED_ROTATED_WEIERSTRASS_H


