//Register GLT2 "MOP_GLT2" MOP,ConOP

#ifndef OFEC_GLT2_H
#define OFEC_GLT2_H

#include "glt.h"

namespace ofec {
	class GLT2 : public GLT {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif
