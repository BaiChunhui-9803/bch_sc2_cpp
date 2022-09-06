#include "cop_base.h"
#include "../../../../../core/algorithm/solution.h"

namespace ofec {
	void CopBase::updateCandidates(const SolBase& sol, std::list<std::unique_ptr<SolBase>>& candidates) const {
		if (candidates.empty())
			candidates.emplace_back(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
		else if (sol.dominate(*candidates.front(), m_id_pro))
			candidates.front().reset(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
	}

	size_t CopBase::numOptimaFound(const std::list<std::unique_ptr<SolBase>>& candidates) const {
		if (!candidates.empty() && candidates.front()->objectiveDistance(m_optima.objective(0)) < m_objective_accuracy)
			return true;
		else
			return false;
	}
}