//Register CEC2013_LSOP_F04 "LSOP_CEC2013_F04" LSOP,ConOP,SOP

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef N7S1_SR_ELLIPTIC_F4_H
#define N7S1_SR_ELLIPTIC_F4_H

#include "function_CEC2013.h"
namespace ofec {
	namespace CEC2013 {
		class N7S1_SR_Elliptic_F4 final:public function_CEC2013 {
		public:
			N7S1_SR_Elliptic_F4(const ParamMap &v);
			N7S1_SR_Elliptic_F4(const std::string &name, size_t size_var, size_t size_obj);
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			~N7S1_SR_Elliptic_F4();

			void initialize();
		};
	}
	using CEC2013_LSOP_F04 = CEC2013::N7S1_SR_Elliptic_F4;
}
#endif
