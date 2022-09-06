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
*
*********************************************************************************/

#ifndef OFEC_DYNAMIC_VAR_H
#define OFEC_DYNAMIC_VAR_H

#include <variant>
#include <string>
#include <map>
#include "../../core/definition.h"
#include <ostream>

namespace ofec {
	using TypeVar = std::variant<bool, int, char, Real, std::string>;

	std::ostream &operator<<(std::ostream &os, const TypeVar &v);

	//class ParamMap
	using ParamMap = std::map<std::string, TypeVar>;
}

#endif /* DYNAMIC_VAR_HPP */
