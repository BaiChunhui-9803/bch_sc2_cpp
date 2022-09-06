//Register WFG4 "MOP_WFG4" MOP,ConOP

#ifndef WFG4_H
#define WFG4_H

#include "WFG.h"

namespace ofec {
	class WFG4 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif
