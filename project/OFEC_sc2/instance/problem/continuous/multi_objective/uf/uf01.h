//Register UF01 "MOP_UF01" MOP,ConOP

#ifndef OFEC_UF1_H
#define OFEC_UF1_H

#include "uf.h"

namespace ofec {
	class UF01 : public UF {
	protected:
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;
	};
}

#endif
