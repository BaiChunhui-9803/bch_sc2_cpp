#ifndef OFEC_HGEA_ADAPTOR_H
#define OFEC_HGEA_ADAPTOR_H

#include <memory>
#include "../../../../../core/problem/encoding.h"

namespace ofec {
	class BaseAdaptorHGEA {
	protected:
		size_t m_num_peaks;
		std::vector<Real> m_potential_peaks;
		std::vector<std::list<const SolBase*>> m_his_sols_peaks;
		std::list<const SolBase*> m_valley_his_sols;
		std::list<std::unique_ptr<const SolBase>> m_his_sols;
		std::vector<std::pair<Real, Real>> m_obj_range;

	public:
		/* Record a newly-evaluated solution  */
		virtual void archiveSolution(const SolBase &sol) = 0;
		/* Update regions of the valley and peaks */
		virtual void updateValleyAndPeaks() = 0;
		/* Return whether the solution is in valley or peaks (-1: valley, >0: the index of peaks) */
		virtual int inValleyOrPeaks(const SolBase &sol) = 0;
		virtual int inValleyOrPeaks(const VarBase &var) = 0;
		/* Replace solution 1 with solution 2 */
		virtual void replaceSolution(SolBase &sol1, const SolBase &sol2) = 0;
		/* Update potenial of each peak */
		virtual void updatePeaksPotentials(int id_pro) = 0;
		/* Update affiliaction of history solutions */
		virtual void updateAffiliationHisSols() = 0;
		/* Return uniqueness of given solutions */
		virtual std::vector<Real> uniquenessSols(std::vector<const SolBase*> &sols) const = 0;
		/* Return adjacent history solutions around the given solution */
		virtual std::vector<const SolBase*> adjacentHisSols(const SolBase &sol2, size_t num, int id_rnd, int id_pro) const = 0;

		size_t numPeaks() const { return m_num_peaks; }
		const std::vector<Real>& potentialPeaks() const { return m_potential_peaks; }
		const std::vector<std::list<const SolBase*>>& hisSolsPeaks() const { return m_his_sols_peaks; }
	};

	template <typename TSol>
	class AdaptorHGEA : public BaseAdaptorHGEA {
	public:
		void archiveSolution(const SolBase &sol) override {
			if (m_obj_range.empty())
				m_obj_range.resize(sol.objectiveSize());
			if (m_his_sols.empty()) {
				for (size_t j = 0; j < sol.objectiveSize(); j++)
					m_obj_range[j].first = m_obj_range[j].second = sol.objective(0);
			}
			else {
				for (size_t j = 0; j < sol.objectiveSize(); j++) {
					if (m_obj_range[j].first > sol.objective(j))
						m_obj_range[j].first = sol.objective(j);
					if (m_obj_range[j].second < sol.objective(j))
						m_obj_range[j].second = sol.objective(j);
				}
			}
			m_his_sols.emplace_back(new TSol(sol));
		}
		
		int inValleyOrPeaks(const SolBase &sol) final override {
			return inValleyOrPeaks(sol.variableBase());
		}

		int inValleyOrPeaks(const VarBase &var) final override {
			return inValleyOrPeaks_(dynamic_cast<const typename TSol::VarEnc&>(var));
		}
		
		void replaceSolution(SolBase &sol1, const SolBase &sol2) final override { 
			dynamic_cast<TSol&>(sol1) = dynamic_cast<const TSol&>(sol2);
		}

	protected:
		virtual int inValleyOrPeaks_(const typename TSol::VarEnc &var) = 0;
	};
}

#endif // !OFEC_HGEA_ADAPTOR_H
