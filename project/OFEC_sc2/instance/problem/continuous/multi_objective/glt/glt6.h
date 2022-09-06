//Register GLT6 "MOP_GLT6" MOP,ConOP

#ifndef OFEC_GLT6_H
#define OFEC_GLT6_H

#include "glt.h"

namespace ofec {
	class GLT6 : public GLT {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif
