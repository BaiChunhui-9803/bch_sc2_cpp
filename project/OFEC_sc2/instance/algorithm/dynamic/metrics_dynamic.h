#ifndef OFEC_METRICS_DYNAMIC_H
#define OFEC_METRICS_DYNAMIC_H

#include "../../../core/algorithm/algorithm.h"
#include "../../../core/problem/solution.h"

namespace ofec{
	class MetricsDynamicConOEA :public Algorithm {
	private:
		void updateCandidates(const SolBase& sol) override;

	protected:
		Real m_offline_error;
		Real m_best_before_change_error;
		int m_num_envirs;
		void initialize_() override;

	public:
		void calculateOfflineError();
		void calculateBBCError();

		Real getOfflineError() const { return m_offline_error; }
		Real getBBCError() const { return m_best_before_change_error; }
	};
}

#endif // !OFEC_METRICS_DYNAMIC_H
