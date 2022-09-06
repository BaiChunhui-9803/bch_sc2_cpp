//Register RotatedScafferF6 "Classic_Scaffer_F6_rotated" GOP,ConOP,SOP

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

#ifndef OFEC_ROTATED_SCAFFER_F6_H
#define OFEC_ROTATED_SCAFFER_F6_H

#include "scaffer_F6.h"

namespace ofec {
	class RotatedScafferF6 : public ScafferF6 {
	protected:
		void initialize_() override;
	};	
}
#endif // ! OFEC_ROTATED_SCAFFER_F6_H

