//Register WFG9 "MOP_WFG9" MOP,ConOP

#ifndef WFG9_H
#define WFG9_H

#include "WFG.h"

namespace ofec {
	class WFG9 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void t3(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif
