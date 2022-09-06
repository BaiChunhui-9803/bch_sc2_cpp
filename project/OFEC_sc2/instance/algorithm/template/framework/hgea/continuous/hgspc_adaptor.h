#ifndef OFEC_HGSPC_ADAPTOR_H
#define OFEC_HGSPC_ADAPTOR_H

#include "../hgea_adaptor.h"
#include "../../../../../../core/problem/solution.h"
#include "../../../../../../utility/kd-tree/kdtree_space.h"

namespace ofec {
	class AdaptorHGSPC : public AdaptorHGEA<Solution<>> {
	public:
		struct InfoSSP {
			int valley_or_peaks;  // -1: valley, >0: the index of peaks
			std::list<const SolBase*> his_sol;
			const SolBase *best_sol = nullptr;
			std::list<size_t> adjacent_ssp;
		};

	protected:
		std::vector<OptMode> m_opt_mode;
		nanoflann::PartitioningKDTree<Real> m_sp_tree;
		std::vector<InfoSSP> m_info_ssp;
		std::vector<size_t> m_ssp_valley;
		std::vector<std::vector<size_t>> m_ssp_peaks;
		std::vector<Real> m_obj_ssp;

	public:
		AdaptorHGSPC(int id_pro, size_t init_num);
		void archiveSolution(const SolBase &sol) override;
		void updateValleyAndPeaks() override;
		int inValleyOrPeaks_(const VarVec<Real> &var) override;
		void updatePeaksPotentials(int id_pro) override;
		void updateAffiliationHisSols() override;
		std::vector<Real> uniquenessSols(std::vector<const SolBase*> &sols) const override;
		std::vector<const SolBase*> adjacentHisSols(const SolBase &sol2, size_t num, int id_rnd, int id_pro) const override;

	private:
		/* for updateValleyAndPeaks() */
		void interpolateMissing();
		void monotonicClustering();
		void monotonicSearching(size_t center, std::set<size_t> &cluster, std::vector<size_t> &num_clutered);
	};
}

#endif // !OFEC_HGSPC_ADAPTOR_H
