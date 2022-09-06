//Register WFG5 "MOP_WFG5" MOP,ConOP

#ifndef WFG5_H
#define WFG5_H

#include "WFG.h"

namespace ofec {
	class WFG5 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif
