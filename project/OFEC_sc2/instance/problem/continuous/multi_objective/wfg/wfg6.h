//Register WFG6 "MOP_WFG6" MOP,ConOP

#ifndef WFG6_H
#define WFG6_H

#include "WFG.h"

namespace ofec {
	class WFG6 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif
