#ifndef OFEC_MMOP_METRICS_H
#define OFEC_MMOP_METRICS_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	class MetricsMMOP : virtual public Continuous {
	protected:
		enum class MeasureBy { kObj, kVar, kObjAndVar };
	public:
		void updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const override;
		size_t numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const override;
		bool isSolved(const std::list<std::unique_ptr<SolBase>> &candidates) const override;

	protected:
		void updateByObj(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const;
		size_t numOptimaByObj(const std::list<std::unique_ptr<SolBase>> &candidates) const;

		void updateByObjAndVar(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const;
		size_t numOptimaByObjAndVar(const std::list<std::unique_ptr<SolBase>> &candidates) const;

		void updateByVar(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const;
		size_t numOptimaByVar(const std::list<std::unique_ptr<SolBase>> &candidates) const;

		void updateBestFixedNumSols(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates, size_t num_sols) const;

		MeasureBy m_mb = MeasureBy::kObj;
	};
}

#endif // !OFEC_MMOP_METRICS_H
