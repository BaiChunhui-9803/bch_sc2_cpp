//Register GLT5 "MOP_GLT5" MOP,ConOP

#ifndef OFEC_GLT5_H
#define OFEC_GLT5_H

#include "glt.h"

namespace ofec {
	class GLT5 : public GLT {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif
