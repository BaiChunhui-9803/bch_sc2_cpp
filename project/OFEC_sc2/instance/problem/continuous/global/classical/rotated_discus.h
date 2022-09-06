//Register RotatedDiscus "Classic_discus_rotated" GOP,ConOP,SOP

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef OFEC_ROTATED_DISCUS_H
#define OFEC_ROTATED_DISCUS_H

#include "discus.h"

namespace ofec {
	class RotatedDiscus : public Discus {
	protected:
		void initialize_() override;
	};
}
#endif // !OFEC_ROTATED_DISCUS_H


