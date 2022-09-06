//Register WFG3 "MOP_WFG3" MOP,ConOP

#ifndef WFG3_H
#define WFG3_H

#include "WFG.h"

namespace ofec {
	class WFG3 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void t3(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif
