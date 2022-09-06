#include "different_powers.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void DifferentPowers::initialize_() {
		Function::initialize_();
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-100., 100.);
		setOriginalGlobalOpt();
		m_optima = m_original_optima;
	}

	void DifferentPowers::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
		obj[0] = 0.0;
		for (size_t i = 0; i<m_num_vars; i++) {
			obj[0] += pow(fabs(x[i]), 2.0 + 4.0*i / (m_num_vars - 1.0));
		}
		obj[0] = pow(obj[0], 0.5);
	}

}