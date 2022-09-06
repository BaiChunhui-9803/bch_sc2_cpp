/* History Learning in Continuous Space */

#ifndef OFEC_HLC_H
#define OFEC_HLC_H

#include "../../../../../utility/kd-tree/kdtree_space.h"
#include "../../../../../utility/heap/median_heap.h"
#include "../../../../../core/problem/solution.h"
#include "../../../../../core/algorithm/individual.h"
#include <memory>
#include <list>
#include <array>
#include <unordered_set>

namespace ofec {
	class HLC {
	public:
		using SplitTree = nanoflann::PartitioningKDTree<Real>;

		struct InfoSubspace {
			std::list<size_t> neighbors;
			Real obj/*, diff*/;
			size_t num_imprv;
			std::list<std::shared_ptr<const Solution<>>> sols;
			std::list<std::shared_ptr<const Solution<>>> survivors;
			//MedianHeap<Real> heap_objs;
			//std::list<std::shared_ptr<const Solution<>>> good_sols;
			//std::list<std::shared_ptr<const Solution<>>> improved_sols;
			std::shared_ptr<const Solution<>> best_sol, worst_sol;
			int aff_bsn_atr = -1;
			std::unique_ptr<SplitTree> atomspace_tree;
		};

		struct InfoBsnAtrct {
			std::vector<size_t> subspaces;
			std::vector<Real> vols_ssps;
			std::vector<Real> prob_ssps;
			std::map<size_t, size_t> seq_ssps;

			std::vector<Real> coverages;           // Coverages of iterations
			std::vector<Real> cuml_covs;           // Cumulative coverages of iterations
			Real log_base;                         // Base number of logarithmic function fitting the cumulative coverage line 
		
			Real fit, diff, div;
			size_t freq;
			Real potential;                        // Normalized potential 
			std::vector<std::unique_ptr<Individual<>>> typ_inds;
			std::vector<double> rho, delta;

			Real volume;
			Real ruggedness;
			std::shared_ptr<const Solution<>> best_sol, worst_sol;
			
			std::map<size_t, std::map<int, size_t>> ssp_atp_to_div;
			std::vector<std::list<std::pair<size_t, int>>> div_to_ssp_atp;

			std::vector<Real> vols_divs;             // Volumes of divisions
			std::vector<int> freq_divs;

			std::vector<std::vector<Real>> cov_divs;
			size_t num_cov_divs_remain_same;

			std::map<size_t, std::vector<Real>> cov_ssps;
			size_t num_covered_ssps_remain;
		};

	private:
		size_t m_size_var;
		std::vector<OptMode> m_opt_mode_sol;
		std::vector<OptMode> m_opt_mode_bsn_atrct;
		std::unique_ptr<SplitTree> m_subspace_tree;
		std::list<std::shared_ptr<const Solution<>>> m_his_inds;
		std::vector<std::unique_ptr<InfoSubspace>> m_info_subspaces;
		std::vector<std::unique_ptr<InfoBsnAtrct>> m_info_bsns_atrct;
		std::shared_ptr<const Solution<>> m_best_sol, m_worst_sol;
		std::unordered_set<size_t> m_interpolated;
		size_t m_num_divs;
		std::list<Solution<>> m_attractors_fnd;

#ifdef OFEC_DEMO  
		bool m_log_param_ready = false;
#endif

		std::vector<std::vector<Real>> m_dis_mat;

	public:
		HLC(size_t size_var, size_t num_divs);
		HLC(const HLC &) = delete;
		void initialize(const std::vector<std::pair<Real, Real>> &boundary, OptMode opt_mode, int init_num);
		void inputSample(const Solution<> &sample_point, bool improved = false);
		void inputSurvivor(const Solution<> &sample_point);
		//void updateImprovedSample(const std::vector<const Solution<>*> &pop);
		void regressCovLines();
		void interpolate();
		size_t clustering();
		std::map<size_t, size_t> clusterInBsnAtrct(size_t id_bsn) const;
		template<typename TMultiPop>
		void updateCurDiversity(TMultiPop &pops, const std::vector<int> &aff_pop, int id_pro, Real best_obj);
		void updatePotential();
		void updateBsnDiff(int id_pro);
		//void updatePotential(int id_pro, int id_rnd, const std::list<Solution<>> &attractors_fnd);
		void updateDPC(size_t id_bsn_atrct, const std::vector<const Solution<>*> &sols, int id_pro, int id_rnd);
		void clearTypInds();
		//void splitBasin(size_t id_bsn_atrct, size_t num_subspaces);
		//void splitBasin(size_t id_bsn_atrct, const std::vector<const Solution<>*> &atrctrs);
		//void splitSubspaces(const std::vector<const Solution<> *> &atrctrs);
		void calCoverage(size_t id_bsn_atrct, const std::vector<const Solution<>*> &sols, int id_pro, Real niche_radius);
		void recordCovDivs(size_t id_bsn_atrct, const std::vector<const Solution<> *> &sols);
		void recordCovSSPs(size_t id_bsn_atrct, const std::vector<const Solution<> *> &sols);
		size_t numDivsFilled(size_t id_bsn_atrct, const std::vector<const Solution<>*> &sols);
		void randomSmpl(size_t id_bsn_atrct, size_t id_div, size_t id_rnd, std::vector<Real>& smpl);
		void randomSmpl(size_t id_bsn_atrct, size_t id_rnd, std::vector<Real> &smpl);
		void randomSmplInSSP(size_t id_subspace, size_t id_rnd, std::vector<Real> &smpl);
		void randomSmplByFreq(size_t id_bsn_atrct, size_t id_rnd, std::vector<Real> &smpl);
		void calRuggedness(size_t id_bsn_atrct, int id_rnd, int id_pro, int id_alg);
		std::list<size_t> divsConverge(size_t id_bsn_atrct) const;
		bool overVisited(size_t id_bsn_atract, const Solution<> &sol, int id_rnd) const;
		void updateAttractorsFnd(const Solution<> &sol, int id_pro);

		const std::list<std::shared_ptr<const Solution<>>> &hisInds() const { return m_his_inds; }
		const SplitTree &subspaceTree() const { return *m_subspace_tree; }
		size_t numSubspaces() const { return m_info_subspaces.size(); }
		const InfoSubspace &infoSubspace(size_t id_subspace) const { return *m_info_subspaces.at(id_subspace); }
		size_t numBsnsAtrct() const { return m_info_bsns_atrct.size(); }
		const InfoBsnAtrct &infoBsnAtrct(size_t id_bsn_atrct) const { return *m_info_bsns_atrct.at(id_bsn_atrct); }
		InfoBsnAtrct &infoBsnAtrct(size_t id_bsn_atrct) { return *m_info_bsns_atrct.at(id_bsn_atrct); }
		const std::unordered_set<size_t>& interpolated() const { return m_interpolated; }
		int getIdxDiv(int id_bsn_atr, const Solution<> &sol) const;
		int getIdxBsn(const Solution<> &sol) const;
		int getIdxSSP(const Solution<> &sol) const;
		const Solution<>& bestSol() const { return *m_best_sol; }
		const Solution<>& worstSol() const { return *m_worst_sol; }
		const std::list<Solution<>>& attractorsFnd() const { return m_attractors_fnd; }

#ifdef OFEC_DEMO  
		bool isLogParamReady()const { return m_log_param_ready; }
		void setLogParamReady(bool flag) { m_log_param_ready = flag; }
#endif

	private:
		//void updateInfoAfterSplitSSP(size_t old_ssp, size_t new_ssp, size_t id_bsn);
		//void updateInfoAfterSplitSSP(size_t old_ssp, size_t new_ssp);
		void divideBsnAtrct(size_t id_bsn_atrct);
		void updateSubspaceInfo(size_t id_ssp, const std::shared_ptr<const Solution<>> &ptr_sol);
		void updateBsnAtrctInfo(size_t id_bsn, const std::shared_ptr<const Solution<>> &ptr_sol);
		void resetSubspaceInfo(size_t id_subspace);
		bool isAdjacent(const std::vector<std::pair<Real, Real>> &box1, const std::vector<std::pair<Real, Real>> &box2);
		void regressCovLine(size_t id_basin);
	};
	
	template<typename TMultiPop>
	void HLC::updateCurDiversity(TMultiPop &pops, const std::vector<int> &aff_pop, int id_pro, Real best_obj) {
		for (size_t id_bsn = 0; id_bsn < m_info_bsns_atrct.size(); ++id_bsn) {
			auto &info_bsn = m_info_bsns_atrct[id_bsn];
			auto &pop = pops[aff_pop[id_bsn]];
			pop.updateBest(id_pro);
			pop.updateWorst(id_pro);
			auto error_best = abs(pop.best().front()->objective(0) - best_obj);
			auto error_worst = abs(pop.worst().front()->objective(0) - best_obj);
			info_bsn->div = log(error_worst) - log(error_best);
		}
	}
}

#endif // !OFEC_HLC_H
