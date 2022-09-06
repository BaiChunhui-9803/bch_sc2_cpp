#include "logarithmic.h"
#include "../../../../../core/problem/solution.h"

namespace ofec {
	RegressLog::RegressLog(const std::vector<Real> &x, const std::vector<Real> &y) : m_x(x), m_y(y) {}

	void RegressLog::updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (candidates.empty())
			candidates.emplace_back(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
		else if (sol.dominate(*candidates.front(), m_id_pro))
			candidates.front().reset(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
	}

	void RegressLog::initialize_() {
		Continuous::initialize_();
		resizeVariable(1);
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		m_domain.setRange(1, 10, 0);
	}
	
	void RegressLog::evaluateObjective(Real* x, std::vector<Real>& obj) {
		obj[0] = 0;
		Real a = 1, b = x[0], c = 1 - a * b;
		for (size_t i = 0; i < m_x.size(); ++i) {
			//obj[0] += pow(a * pow(b, m_y[i]) + c - m_x[i], 2);
			obj[0] += pow(a * pow(b, m_y[i]) + c - m_x[i], 2) / pow(i + 1, 2);
		}
	}
}