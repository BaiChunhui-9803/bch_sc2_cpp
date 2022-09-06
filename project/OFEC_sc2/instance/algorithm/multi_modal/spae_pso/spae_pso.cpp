#include "spae_pso.h"

namespace ofec {
	void SPAE_PSO::initialize_() {
		ContSPAE::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_w = v.count("weight") > 0 ? std::get<Real>(v.at("weight")) : 0.721;
		m_c1 = v.count("accelerator1") > 0 ? std::get<Real>(v.at("accelerator1")) : 1.193;
		m_c2 = v.count("accelerator2") > 0 ? std::get<Real>(v.at("accelerator2")) : 1.193;
	}

	void SPAE_PSO::initSubPop(Swarm<Particle11> &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg, int id_rnd) {
		ContSPAE::initSubPop(pop, id_atr_bsn, hlc, id_pro, id_alg, id_rnd);
		pop.W() = m_w;
		pop.C1() = m_c1;
		pop.C2() = m_c2;
		pop.initPbest(m_id_pro);
		pop.initVelocity(m_id_pro, m_id_rnd);
	}

	void SPAE_PSO::setPopType() {
		m_pop_type = "SPSO11";
	}

	bool SPAE_PSO::isPopConverged(Swarm<Particle11> &pop) {
		//for (size_t i = 0; i < pop.size(); ++i) {
		//	if (pop[i].speed() > 1e-4)
		//		return false;
		//}
		//return true;
		if (pop.iteration() - pop.timeBestUpdated() < 10)
			return false;
		else
			return true;
	}

	void SPAE_PSO::initInd(HLC &hlc, int id_pro, int id_alg, size_t id_atr_bsn, size_t id_div, IndType &ind) {
		ContSPAE::initInd(hlc, id_pro, id_alg, id_atr_bsn, id_div, ind);
		ind.pbest() = ind.solut();
		ind.initVelocity(id_pro, m_id_rnd);
	}
}