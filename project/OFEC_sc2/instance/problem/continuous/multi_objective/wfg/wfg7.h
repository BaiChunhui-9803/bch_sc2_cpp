//Register WFG7 "MOP_WFG7" MOP,ConOP

#ifndef WFG7_H
#define WFG7_H

#include "WFG.h"

namespace ofec {
	class WFG7 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void t3(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif
