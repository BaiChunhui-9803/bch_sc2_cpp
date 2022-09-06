//Register CEC2005_GOP_F23 "GOP_CEC2005_F23" GOP,ConOP,SOP

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

#ifndef OFEC_F23_NON_CONTINUOUS_ROTATED_HYBRID_H
#define OFEC_F23_NON_CONTINUOUS_ROTATED_HYBRID_H

#include "composition.h"
#include "../metrics_gop.h"

namespace ofec {
	namespace cec2005 {
		class F23_non_continuous_rotated_hybrid final : public Composition, public MetricsGOP {
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction() override;
		};
	}
	using CEC2005_GOP_F23 = cec2005::F23_non_continuous_rotated_hybrid;
}
#endif // !OFEC_F23_NON_CONTINUOUS_ROTATED_HYBRID_H
