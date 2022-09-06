#include "metrics_mop.h"
#include "../../../../core/problem/solution.h"
#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	void MetricsMOP::updateCandidates(const SolBase& sol, std::list<std::unique_ptr<SolBase>>& candidates) const {
		bool dominated = false;
		Dominance result;
		for (auto it = candidates.begin(); it != candidates.end();) {
			result = sol.compare(**it, m_opt_mode);
			if (result == Dominance::kDominated) {
				dominated = true;
				break;
			}
			if (result == Dominance::kDominant) {
				it = candidates.erase(it);
			}
			else
				it++;
		}
		if (!dominated)
			candidates.emplace_back(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
	}

	size_t MetricsMOP::numOptimaFound(const std::list<std::unique_ptr<SolBase>>& candidates) const {
		return 0;
	}
}