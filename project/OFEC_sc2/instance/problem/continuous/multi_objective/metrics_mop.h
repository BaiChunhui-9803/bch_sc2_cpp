#ifndef OFEC_METRICS_MOP_H
#define OFEC_METRICS_MOP_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	class MetricsMOP : virtual public Continuous {
	public:
		void updateCandidates(const SolBase& sol, std::list<std::unique_ptr<SolBase>>& candidates) const override;
		size_t numOptimaFound(const std::list<std::unique_ptr<SolBase>>& candidates) const override;
	};
}
#endif // !OFEC_METRICS_MOP_H