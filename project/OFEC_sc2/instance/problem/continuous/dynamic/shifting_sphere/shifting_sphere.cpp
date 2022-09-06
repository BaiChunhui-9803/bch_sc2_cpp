#include "shifting_sphere.h"
#include "../../../../../core/instance_manager.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif // OFEC_DEMO


namespace ofec {
	void ShfitingSphere::initialize_() {
		Sphere::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_change_freq = std::get<int>(v.at("change frequency"));
	}

	int ShfitingSphere::updateEvalTag(SolBase &s, int id_alg, bool effective_eval) {
		if (id_alg > -1 && GET_ALG(id_alg).numEffectiveEvals() % m_change_freq == 0 && GET_ALG(id_alg).numEffectiveEvals() > 0 && effective_eval) {
			std::vector<Real> rand_trans(m_num_vars);
			for (size_t j = 0; j < m_num_vars; j++) {
				Real mean = (m_domain[j].limit.first + m_domain[j].limit.second) / 2;
				Real std = (m_domain[j].limit.second - m_domain[j].limit.first) / 3;
				rand_trans[j] = GET_RND(m_id_rnd).normal.nextNonStd(mean, std);
			}
			setTranslation(rand_trans.data());
			setGlobalOpt(rand_trans.data());
#ifdef OFEC_DEMO
			Demo::g_buffer->appendProBuffer(m_id_pro);
#endif
			return kChangeNextEval;
		}
		else
			return Continuous::updateEvalTag(s, id_alg, effective_eval);
	}
}