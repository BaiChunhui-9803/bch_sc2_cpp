//Register CEC2005_GOP_F18 "GOP_CEC2005_F18" GOP,ConOP,SOP

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

#ifndef OFEC_F18_ROTATED_HYBRID_COMPOSITION_H
#define OFEC_F18_ROTATED_HYBRID_COMPOSITION_H

#include "composition.h"
#include "../metrics_gop.h"

namespace ofec {
	namespace cec2005 {
		class RotatedHybridComposition2 final : public Composition, public MetricsGOP {
		protected:
			bool loadTranslation(const std::string &path) override;
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction() override;
			void setTranslation() override;
		};
	}
	using CEC2005_GOP_F18 = cec2005::RotatedHybridComposition2;
}
#endif // !OFEC_F18_ROTATED_HYBRID_COMPOSITION_H
