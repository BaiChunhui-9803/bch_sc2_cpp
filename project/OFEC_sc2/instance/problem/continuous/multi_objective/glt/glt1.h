//Register GLT1 "MOP_GLT1" MOP,ConOP

#ifndef OFEC_GLT1_H
#define OFEC_GLT1_H

#include "glt.h"

namespace ofec {
	class GLT1 : public GLT {
	protected:
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;
	};
}

#endif