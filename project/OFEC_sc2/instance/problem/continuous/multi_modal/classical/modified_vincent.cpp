#include "modified_vincent.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void ModifiedVincent::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-10, 10);
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		// 6^Dim gopt 
		m_optima.clear();
		size_t num = (int)pow(6, m_num_vars);
		for (size_t i = 0; i < num; i++)
			m_optima.appendObj(0);
		m_optima.setObjectiveGiven(true);
	}

	void ModifiedVincent::evaluateObjective(Real *x, std::vector<Real> &obj) {
		size_t i;
		obj[0] = 0.0;
		for (i = 0; i < m_num_vars; i++) {
			x[i] += 4.1112;
			if ((x[i] >= 0.25) & (x[i] <= 10.0))
			{
				obj[0] += -sin(10.0 * log(x[i]));
			}
			else if (x[i] < 0.25)
			{
				obj[0] += pow(0.25 - x[i], 2.0) - sin(10.0 * log(2.5));
			}
			else
			{
				obj[0] += pow(x[i] - 10, 2.0) - sin(10.0 * log(10.0));
			}
		}
		obj[0] /= m_num_vars;
		obj[0] += 1.0;
	}

}