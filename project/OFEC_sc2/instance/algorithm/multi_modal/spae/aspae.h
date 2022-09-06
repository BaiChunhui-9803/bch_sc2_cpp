//Register ASPAE "ASPAE" ConOP,SOP,GOP,MMOP

#include "../../../../core/algorithm/algorithm.h"
#include "../../template/framework/spae/hlc.h"
#include "../../template/classic/de/de_pop.h"
#include "../../global/cma_es/cmaes_pop.h"
#include "../../../../core/algorithm/multi_population.h"

namespace ofec {
	//class PopExploit : public PopCMAES {
	//public:
	//	void initializeInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_rnd, const std::vector<Real> &xstart, const std::vector<Real> &stddev);
	//	int evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd);
	//protected:
	//	void reproduceInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_rnd);
	//};

	class ASPAE : public Algorithm {
	protected:
		class PopExplore : public PopDE<> {
		public:
			PopExplore() = default;
			PopExplore(size_t size, int id_pro) : PopDE<>(size, id_pro) {}
			int evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd, const std::list<Solution<>> &attractors_fnd);
		};

		class PopExploit : public PopDE<> {
		public:
			PopExploit(size_t size, int id_pro) : PopDE<>(size, id_pro) {}
			int evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd);
		};

	public:
		enum class Stage { explore, cluster, exploit };
	protected:
		size_t m_num_ssps, m_pop_size, m_subpop_size;
		std::unique_ptr<HLC> m_hlc;
		PopExplore m_main_pop;
		//std::vector<size_t> m_aff_bsn;
		MultiPopulation<PopExploit> m_subpops;
		std::list<Solution<>> m_attractors_fnd;
		int m_cur_bsn;
		std::vector<VarVec<Real>> m_centers;
		std::vector<Real> m_radius, m_init_radius;
		//Real m_converg_radius;

		void initialize_() override;
		void run_() override;
		void initSubpop(size_t k, const std::vector<size_t> &cluster);
		void updateCenterRadius(size_t k);
		bool isOverlapping(size_t k1, size_t k2);
		void mergeSubpops(size_t k1, size_t k2);
		//bool isSubpopOverSearch(size_t k);
		bool isSubpopConverged(size_t k);
		void disturbSubPop(size_t k);
		void updateAttrctrsFnd(size_t k);
		void recordCovSSPs();

	public:
		void record() override;
#ifdef OFEC_DEMO
		const HLC& getHLC() const { return *m_hlc; }
		int curBsn()const { return m_cur_bsn; }
#endif // OFEC_DEMO
	};
}