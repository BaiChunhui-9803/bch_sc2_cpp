#include  "f6_shubert.h"
#include "../../../../../core/instance_manager.h"

namespace ofec::cec2013 {
	void Shubert::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		if (m_num_vars != 2 && m_num_vars != 3)
			throw MyExcept("number of variables of F6_shubert should be 2 or 3");
		setDomain(-10, 10);
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (ofec::Real)1.e-4;
		m_variable_niche_radius = 0.5;

		m_optima.clear();
		size_t num_global_optima = m_num_vars * (size_t)pow(3, m_num_vars);
		if (m_num_vars == 2) {
			for (size_t i = 0; i < num_global_optima; ++i)
				m_optima.appendObj(186.7309088);
		}
		else {
			for (size_t i = 0; i < num_global_optima; ++i)
				m_optima.appendObj(2709.093505);
		}
		m_optima.setObjectiveGiven(true);
	}

	void Shubert::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 1;
		for (int i = 0; i < m_num_vars; ++i) {
			Real a = 0;
			for (int j = 1; j <= 5; ++j) {
				a += j * cos((j + 1)*x[i] + j);
			}
			s *= a;
		}
		obj[0] = -s;
	}
}