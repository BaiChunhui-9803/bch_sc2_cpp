//Register CEC2005_GOP_F04 "GOP_CEC2005_F04" GOP,ConOP,SOP

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


#ifndef OFEC_F4_SHIFTED_SCHWEFEL_1_2_NOISY_H
#define OFEC_F4_SHIFTED_SCHWEFEL_1_2_NOISY_H

#include "../classical/schwefel_1_2.h"

namespace ofec {
	namespace cec2005 {
		class ShiftedSchwefel_1_2_Noisy final : public Schwefel_1_2 {	
		protected:
			void initialize_() override;
			void evaluateOriginalObj(Real *x, std::vector<Real>& obj) override;
		};
	}
	using CEC2005_GOP_F04 = cec2005::ShiftedSchwefel_1_2_Noisy;
}
#endif // ! OFEC_F4_SHIFTED_SCHWEFEL_1_2_NOISY_H
