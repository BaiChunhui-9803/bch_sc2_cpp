//Register SPAE "SPAE" ConOP,SOP,GOP,MMOP

#include "../../../../core/algorithm/algorithm.h"
#include "../../template/framework/spae/hlc.h"
#include "../../template/classic/de/de_pop.h"
#include "../../../../core/algorithm/multi_population.h"

namespace ofec {
	class SPAE : public Algorithm {
	protected:
		class PopExploit : public PopDE<> {
		public:
			PopExploit(size_t size, int id_pro) : PopDE<>(size, id_pro) {}
			int evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd);
		};

		using PopType = PopExploit;

		size_t m_num_ssps, m_subpop_size;
		std::unique_ptr<HLC> m_hlc;
		MultiPopulation<PopType> m_subpops;

		std::vector<int> m_aff_pop, m_aff_bsn;
		
		void initialize_() override;
		void run_() override;

		bool isSubpopConverged(const PopType &subpop);
		void updateAttrctrsFnd(const PopType &subpop);
		void updateAffiliatedSubPop();
		void initSubpopInBsn(size_t id_bsn);
		bool isOverVisited(const Solution<> &sol, size_t id_bsn);

	public:
		void record() override {}
#ifdef OFEC_DEMO
		const HLC &getHLC() const { return *m_hlc; }
		void updateBuffer();
#endif // !OFEC_DEMO

	};
}