//Register UF10 "MOP_UF10" MOP,ConOP

#ifndef OFEC_UF10_H
#define OFEC_UF10_H

#include "uf.h"

namespace ofec {
	class UF10 : public UF {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif

