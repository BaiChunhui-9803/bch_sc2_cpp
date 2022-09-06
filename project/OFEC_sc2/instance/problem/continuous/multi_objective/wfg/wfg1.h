//Register WFG1 "MOP_WFG1" MOP,ConOP

#ifndef WFG1_H
#define WFG1_H

#include "WFG.h"

namespace ofec {
	class WFG1 final : public WFG {
	protected:
		void t1(std::vector<Real> &y) override;
		void t2(std::vector<Real> &y) override;
		void t3(std::vector<Real> &y) override;
		void t4(std::vector<Real> &y) override;
		void shape(std::vector<Real> &y) override;
	};
}


#endif