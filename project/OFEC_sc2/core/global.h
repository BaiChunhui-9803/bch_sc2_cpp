/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class globla defines global objects used in OFEC
*
*
*********************************************************************************/

#ifndef OFEC_GLOBAL_H
#define OFEC_GLOBAL_H

#include <set>
#include <map>
#include <mutex>

#include "algorithm/algorithm.h"
#include "problem/problem.h"
#include "../utility/factory.h"

namespace ofec {

	extern std::map<std::string, std::set<std::string>> g_alg_for_pro;
	extern std::set<std::string> g_param_omit;
	extern std::map<std::string, std::string> g_param_abbr;
	extern std::string g_working_dir;
	extern std::mutex g_cout_mutex;

	extern Factory<Problem> g_reg_problem;
	extern Factory<Algorithm> g_reg_algorithm;
}
#endif // !OFEC_GLOBAL_H

