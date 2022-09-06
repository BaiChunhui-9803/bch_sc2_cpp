#include "metrics_gop.h"
#include "../../../../core/problem/solution.h"
#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	void MetricsGOP::updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (candidates.empty())
			candidates.emplace_back(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
		else if (sol.dominate(*candidates.front(), m_id_pro))
			candidates.front().reset(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
	}

	size_t MetricsGOP::numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (!candidates.empty() && candidates.front()->objectiveDistance(m_optima.objective(0)) < m_objective_accuracy)
			return true;
		else
			return false;
	}
}