//Register GLT4 "MOP_GLT4" MOP,ConOP

#ifndef OFEC_GLT4_H
#define OFEC_GLT4_H

#include "glt.h"

namespace ofec {
	class GLT4 : public GLT {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif
