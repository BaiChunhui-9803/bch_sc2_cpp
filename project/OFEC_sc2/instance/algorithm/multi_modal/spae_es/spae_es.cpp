#include "spae_es.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void SPAE_ES::setPopType() {
		m_pop_type = "CMA-ES";
	}

	void SPAE_ES::initSubPop(PopCMAES &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg, int id_rnd) {
		ContSPAE::initSubPop(pop, id_atr_bsn, hlc, id_pro, id_alg, id_rnd);
		std::vector<Real> xmean(GET_CONOP(id_pro).numVariables()); 
		std::vector<Real> stddev(GET_CONOP(id_pro).numVariables());
		std::vector<Real> data(pop.size());
		for (size_t j = 0; j < GET_CONOP(id_pro).numVariables(); j++) {
			for (size_t i = 0; i < pop.size(); i++)
				data[i] = pop[i].variable()[j];
			calMeanAndStd(data, xmean[j], stddev[j]);
			stddev[j] /= 2;
		}
		pop.initializeByNonStd(id_pro, id_rnd, xmean, stddev);
	}
}
