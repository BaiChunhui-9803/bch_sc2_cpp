#include "f4_himmenblau.h"
#include "../../../../../core/instance_manager.h"
#include "../metrics_mmop.h"

namespace ofec::cec2013 {
	void Himmenblau::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;

		resizeVariable(2);
		setDomain(-6, 6);

		m_optima.clear();
		for (size_t i = 0; i < 4; i++)
			m_optima.appendObj(200.0);
		m_optima.setObjectiveGiven(true);
		auto &v = GET_PARAM(m_id_param);
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (ofec::Real)1.e-4;
		m_variable_niche_radius = 0.01;
	}

	void Himmenblau::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		Real t0 = (x[0] * x[0] + x[1] - 11), t1 = (x[1] * x[1] + x[0] - 7);
		s = 200 - (t0*t0 + t1 * t1);
		obj[0] = s;
	}
}