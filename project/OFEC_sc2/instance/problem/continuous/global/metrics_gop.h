#ifndef OFEC_METRICS_GOP_H
#define OFEC_METRICS_GOP_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec{
	class MetricsGOP : virtual public Continuous {
	public:
		void updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const override;
		size_t numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const override;
	};
}

#endif // !OFEC_METRICS_GOP_H
