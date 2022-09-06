#include "UDF4.h"

namespace ofec {
	UDF4::UDF4(param_map & v) : UDF4(v.at("problem name"), v.at("number of variables")) {

	}
	UDF4::UDF4(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void UDF4::initialize() {
		for (size_t i = 0; i < m_num_vars; ++i) {//the first one is in [0,1], others are in [-1,1]
			if (i < 1)
				m_domain.setDomain(0., 1., i);
			else
				m_domain.setDomain(-1., 1., i);
		}
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//recommend n=20;
	void UDF4::generateAdLoadPF() {
		int num = 1000;
		Real t = get_time();
		Real Mt = 0.5+fabs(sin(0.5*OFEC_PI*t));
		Real Ht = 0.5 + std::fabs(std::sin(0.5*OFEC_PI*t));
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0 + i * 1. / (num-1);
			o_point[i][1] = 1 - Mt*std::pow(o_point[i][0],Ht);
			m_optima.append(o_point[i].vect());
		}
	}

	int UDF4::evaluateObjective(Real *x, std::vector<Real> &obj) {//recommend the number of variables is 20
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real Mt = 0.5 + std::fabs(std::sin(0.5 * OFEC_PI*t));//Angular shift in PF
		Real Ht = 0.5 + std::fabs(std::sin(0.5 * OFEC_PI*t));//Curvature variation in PF
		Real Kt = std::ceil(m_num_vars*std::sin(0.5 * OFEC_PI*t));//Phase shifting in trigonometric type of PS

		for (size_t m = 0; m < m_num_objs; ++m)
		{
			if (m < 1) {
				obj[m] = 0;
				for (size_t i=2; i < m_num_vars; i += 2) {
					obj[m] += std::pow(x[i] - std::sin(6 * OFEC_PI*x[0] + (i + 1+Kt) * OFEC_PI / m_num_vars), 2);
				}
				obj[m] = x[0] + 2. / (ceil(m_num_vars / 2.) - 1)*obj[m];
			}
			else {
				obj[m] = 0;
				for (size_t i=1; i < m_num_vars; i += 2) {
					obj[m] += std::pow(x[i] - std::sin(6 * OFEC_PI*x[0] + (i + 1 + Kt) * OFEC_PI / m_num_vars), 2);
				}
				obj[m] = 1 - Mt*std::pow(x[0],Ht) + 2. / (floor(m_num_vars / 2.))*obj[m];
			}
		}
		return kNormalEval;
	}
}