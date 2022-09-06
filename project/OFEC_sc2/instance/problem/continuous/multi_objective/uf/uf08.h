//Register UF08 "MOP_UF08" MOP,ConOP

#ifndef OFEC_UF8_H
#define OFEC_UF8_H

#include "uf.h"

namespace ofec {
	class UF08 : public UF {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif
