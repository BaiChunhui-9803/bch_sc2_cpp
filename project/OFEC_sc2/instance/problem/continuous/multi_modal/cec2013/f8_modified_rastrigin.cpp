#include "f8_modified_rastrigin.h"
#include "../../../../../core/instance_manager.h"

namespace ofec::cec2013 {
	void ModifiedRastrigin::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		resizeVariable(2);
		setDomain(0, 1);
		m_k.resize(2);
		m_k[0] = 3;
		m_k[1] = 4;
		m_variable_niche_radius = 0.01;
		auto &v = GET_PARAM(m_id_param);
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (ofec::Real)1.e-4;

		m_optima.clear();
		size_t num_global_optima = 1;
		for (size_t i = 0; i < 2; i++)
			num_global_optima *= m_k[i];
		for (size_t i = 0; i < num_global_optima; ++i)
			m_optima.appendObj(-2.);
		VarVec<Real> opt_var(2);
		for (size_t i = 0; i < m_k[0]; i++)	{
			opt_var[0] = (i * 2. + 1) / (m_k[0] * 2);
			for (size_t j = 0; j < m_k[1]; j++) {
				opt_var[1] = (j * 2. + 1) / (m_k[1] * 2);
				m_optima.appendVar(opt_var);
			}
		}
		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
	}

	void ModifiedRastrigin::evaluateObjective(Real* x, std::vector<Real>& obj) {
		Real s = 0;
		for (int i = 0; i < m_num_vars; ++i) {
			s += 10 + 9 * cos(2 * OFEC_PI * m_k[i] * x[i]);
		}
		obj[0] = -s;
	}
}