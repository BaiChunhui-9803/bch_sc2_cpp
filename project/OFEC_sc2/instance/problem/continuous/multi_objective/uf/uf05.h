//Register UF05 "MOP_UF05" MOP,ConOP

#ifndef OFEC_UF5_H
#define OFEC_UF5_H

#include "uf.h"

namespace ofec {
	class UF05 : public UF {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif

