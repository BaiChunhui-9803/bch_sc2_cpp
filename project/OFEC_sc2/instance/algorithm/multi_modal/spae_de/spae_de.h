//Register SPAE_DE "SPAE-DE" ConOP,SOP,GOP,MMOP
//Register SPAE_NRandDE "SPAE-NRandDE" ConOP,SOP,GOP,MMOP
//Register SPAE_RemixDE "SPAE-RemixDE" ConOP,SOP,GOP,MMOP

#ifndef OFEC_SPAE_DE_H
#define OFEC_SPAE_DE_H

#include "../../template/framework/spae/spae_cont.h"
#include "../../template/classic/de/de_pop.h"
#include "../de_nrand_1/de_nrand_1_pop.h"
#include "../nsde/nsde.h"
#include "../ncde/ncde.h"
#include "../../global/cma_es/cmaes_pop.h"

namespace ofec {
	class SPAE_DE : public ContSPAE<PopDE<>> {
	protected:
		Real m_f, m_cr;
		MutationDE m_ms;

		void initialize_() override;
		void setPopType() override;
		//bool isPopConverged(PopDE<> &pop) override;
		void initSubPop(PopDE<> &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg, int id_rnd) override;
		void evolveSubPop(PopDE<> &pop) override;
	};

	class SPAE_NRandDE : public ContSPAE<PopNRand1DE> {
	protected:
		void setPopType() override;
		void evolveSubPop(PopNRand1DE &pop) override;
	};


	class SPAE_RemixDE : public BaseContSPAE {
		class PopExploit : public PopCMAES {
		public:
			PopExploit() = default;
			PopExploit(size_t size_pop, int id_pro) : PopCMAES(size_pop, id_pro) {}
			int evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd);
		protected:
			void reproduceInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_rnd);
		};

		class PopExplore : public PopDE<> {
		public:
			int evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd);
		};

	protected:
		size_t m_pop_size;
		PopExplore m_pop;
		MultiPopulation<PopExploit> m_subpop;

		//MultiPopulation<PopDE<>> m_subpop;
		//Population<Individual<>> m_pop;

		Real m_threshold;

		//MultiPopulation<PopCMAES> m_subpop;
		std::list<size_t> m_bsn_to_search;
		std::list<std::unique_ptr<Solution<>>> m_converged;
		std::vector<bool> m_divs_converged;

		void initialize_() override;
		void run_() override;
		void initPop();
		void initSubpop();
		bool isSubpopOutBsnAtrct(size_t k);
		bool isSubpopOverSearch(size_t k);
		bool isSubpopConverged(size_t k);
		void updateConverged(size_t k);

	public:
		void record() override;
	};
}

#endif //!OFEC_SPAE_DE_H