//Register CEC2013_MMOP_F01 "MMOP_CEC2013_F01" MMOP,ConOP,SOP
//Register CEC2013_MMOP_F02 "MMOP_CEC2013_F02" MMOP,ConOP,SOP
//Register CEC2013_MMOP_F03 "MMOP_CEC2013_F03" MMOP,ConOP,SOP
//Register CEC2013_MMOP_F04 "MMOP_CEC2013_F04" MMOP,ConOP,SOP
//Register CEC2013_MMOP_F05 "MMOP_CEC2013_F05" MMOP,ConOP,SOP
//Register CEC2013_MMOP_F06 "MMOP_CEC2013_F06" MMOP,ConOP,SOP
//Register CEC2013_MMOP_F07 "MMOP_CEC2013_F07" MMOP,ConOP,SOP
//Register CEC2013_MMOP_F08 "MMOP_CEC2013_F08" MMOP,ConOP,SOP

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
******************************************************************************************
*  Paper: Benchmark Functions for CEC��2013 Special Session and Competition on Niching
*  Methods for Multimodal Function Optimization.
*******************************************************************************************/


#ifndef OFEC_F1_F8_H
#define OFEC_F1_F8_H

#include "../classical/five_uneven_peak_trap.h"
#include "../classical/equal_maxima.h"
#include "../classical/uneven_de_maxima.h"
#include "f4_himmenblau.h"
#include "f5_six_hump_camel_back.h"
#include "f6_shubert.h"
#include "../classical/vincent.h"
#include "f8_modified_rastrigin.h"

namespace ofec {
	namespace cec2013 {
		using F1_five_uneven_peak_trap = FiveUnevenPeakTrap;
		using F2_equal_maxima = EqualMaxima;
		using F3_uneven_de_maxima = UnevenDeMaxima;
		using F7_vincent = Vincent;
	}
	using CEC2013_MMOP_F01 = cec2013::F1_five_uneven_peak_trap;
	using CEC2013_MMOP_F02 = cec2013::F2_equal_maxima;
	using CEC2013_MMOP_F03 = cec2013::F3_uneven_de_maxima;
	using CEC2013_MMOP_F04 = cec2013::Himmenblau;
	using CEC2013_MMOP_F05 = cec2013::SixHumpCamelBack;
	using CEC2013_MMOP_F06 = cec2013::Shubert;
	using CEC2013_MMOP_F07 = cec2013::F7_vincent;
	using CEC2013_MMOP_F08 = cec2013::ModifiedRastrigin;
}
#endif // !OFEC_F1_F8_H