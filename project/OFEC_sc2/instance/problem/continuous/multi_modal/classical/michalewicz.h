//Register Michalewicz "Classic_Michalewicz" MMOP,ConOP,SOP

#ifndef OFEC_MICHALEWICZ_H
#define OFEC_MICHALEWICZ_H

#include "../metrics_mmop.h"

namespace ofec {	
	class Michalewicz : public MetricsMMOP {
	protected:
		void initialize_();
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;

		int m_m;
	};
}
#endif // !OFEC_MICHALEWICZ_H