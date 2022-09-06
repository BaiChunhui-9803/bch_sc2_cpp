//Register CEC2013_LSOP_F12 "LSOP_CEC2013_F12" LSOP,ConOP,SOP

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

#ifndef SHIFTEDROSENBROCK_F12_H
#define SHIFTEDROSENBROCK_F12_H

#include "function_CEC2013.h"
namespace ofec {
	namespace CEC2013 {
		class ShiftedRosenbrock_F12 final:public function_CEC2013 {
		public:
			ShiftedRosenbrock_F12(const ParamMap &v);
			ShiftedRosenbrock_F12(const std::string &name, size_t size_var, size_t size_obj);
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			~ShiftedRosenbrock_F12();

			void initialize();
		};
	}
	using CEC2013_LSOP_F12 = CEC2013::ShiftedRosenbrock_F12;
}
#endif





