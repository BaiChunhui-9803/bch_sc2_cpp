//Register GLT3 "MOP_GLT3" MOP,ConOP

#ifndef OFEC_GLT3_H
#define OFEC_GLT3_H

#include "glt.h"

namespace ofec {
	class GLT3 : public GLT {
	protected:
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
	};
}

#endif