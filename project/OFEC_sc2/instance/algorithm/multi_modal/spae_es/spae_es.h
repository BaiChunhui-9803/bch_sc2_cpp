//Register SPAE_ES "SPAE-ES" GOP,MMOP,ConOP,SOP

#ifndef OFEC_SPAE_ES_H
#define OFEC_SPAE_ES_H

#include "../../global/cma_es/cmaes_pop.h"
#include "../../template/framework/spae/spae_cont.h"

namespace ofec {
	class SPAE_ES : public ContSPAE<PopCMAES> {
	protected:
		void setPopType() override;
		void restrictPop(PopCMAES &pop, size_t id_atr_bsn) override {}
		//bool isPopConverged(PopCMAES &pop) override;
		void initSubPop(PopCMAES &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg, int id_rnd) override;
	};
}

#endif // !OFEC_SPAE_ES_H

