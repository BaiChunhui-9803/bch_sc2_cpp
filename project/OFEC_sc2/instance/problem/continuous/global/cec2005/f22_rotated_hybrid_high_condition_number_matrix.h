//Register CEC2005_GOP_F22 "GOP_CEC2005_F22" GOP,ConOP,SOP

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

#ifndef OFEC_F22_ROTATED_HYBRID_HIGH_CONDITION_NUMBER_MATRIX_H
#define OFEC_F22_ROTATED_HYBRID_HIGH_CONDITION_NUMBER_MATRIX_H

#include "composition.h"
#include "../metrics_gop.h"

namespace ofec {
	namespace cec2005 {
		class RotatedHybridHighConditionNumberMatrix final : public Composition, public MetricsGOP {
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction() override;
		};
	}
	using CEC2005_GOP_F22 = cec2005::RotatedHybridHighConditionNumberMatrix;
}
#endif // !OFEC_F22_ROTATED_HYBRID_HIGH_CONDITION_NUMBER_MATRIX_H
