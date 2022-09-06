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

#ifndef OFEC_CEC2015_COMPOSITION_H
#define OFEC_CEC2015_COMPOSITION_H

#include "../CEC2005/composition.h"

namespace ofec {
	namespace CEC2015 {
		class composition_2015 : public CEC2005::Composition {
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void set_weight(std::vector<Real>& w, const std::vector<Real>&x);
		protected:
			std::vector<Real> m_f_bias;
		};
	}
}
#endif // ! OFEC_CEC2015_COMPOSITION_H
