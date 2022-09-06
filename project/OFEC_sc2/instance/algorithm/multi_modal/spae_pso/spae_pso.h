//Register SPAE_PSO "SPAE-PSO" ConOP,SOP,GOP,MMOP

#ifndef OFEC_SPAE_PSO_H
#define OFEC_SPAE_PSO_H

#include "../../template/framework/spae/spae_cont.h"
#include "../../global/spso11/spso11.h"

namespace ofec {
	class SPAE_PSO : public ContSPAE<Swarm<Particle11>> {
	protected:
		Real m_w, m_c1, m_c2;

		void initialize_() override;
		void setPopType() override;
		bool isPopConverged(Swarm<Particle11> &pop) override;
		void initSubPop(Swarm<Particle11> &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg, int id_rnd) override;
		void initInd(HLC &hlc, int id_pro, int id_alg, size_t id_atr_bsn, size_t id_div, IndType &ind) override;
	};
}

#endif