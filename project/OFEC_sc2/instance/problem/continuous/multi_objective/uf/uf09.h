//Register UF09 "MOP_UF09" MOP,ConOP

#ifndef OFEC_UF9_H
#define OFEC_UF9_H

#include "uf.h"

namespace ofec {
	class UF09 : public UF {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif
