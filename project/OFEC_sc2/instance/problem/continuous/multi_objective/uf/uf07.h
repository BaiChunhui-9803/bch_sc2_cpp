//Register UF07 "MOP_UF07" MOP,ConOP

#ifndef OFEC_UF7_H
#define OFEC_UF7_H

#include "uf.h"

namespace ofec {
	class UF07 : public UF {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif
