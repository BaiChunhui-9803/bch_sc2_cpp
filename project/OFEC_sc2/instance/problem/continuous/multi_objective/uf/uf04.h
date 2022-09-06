//Register UF04 "MOP_UF04" MOP,ConOP

#ifndef OFEC_UF4_H
#define OFEC_UF4_H

#include "uf.h"

namespace ofec {
	class UF04 : public UF{
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif

