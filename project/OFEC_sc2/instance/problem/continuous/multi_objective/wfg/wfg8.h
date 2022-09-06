//Register WFG8 "MOP_WFG8" MOP,ConOP

#ifndef WFG8_H
#define WFG8_H

#include "WFG.h"

namespace ofec {
	class WFG8 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void t3(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif
