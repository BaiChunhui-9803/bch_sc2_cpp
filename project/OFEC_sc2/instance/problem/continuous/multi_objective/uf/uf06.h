//Register UF06 "MOP_UF06" MOP,ConOP

#ifndef OFEC_UF6_H
#define OFEC_UF6_H

#include "uf.h"

namespace ofec {
	class UF06 : public UF {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif

