//Register WFG2 "MOP_WFG2" MOP,ConOP

#ifndef WFG2_H
#define WFG2_H

#include "WFG.h"

namespace ofec {
	class WFG2 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void t3(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif
