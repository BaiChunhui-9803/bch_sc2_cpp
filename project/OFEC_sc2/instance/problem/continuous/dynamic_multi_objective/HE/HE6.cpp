#include "HE6.h"

namespace ofec {
	HE6::HE6(param_map & v) : HE6(v.at("problem name"), v.at("number of variables")) {

	}
	HE6::HE6(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void HE6::initialize() {
		for (size_t i = 0; i < m_num_vars; ++i) {
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

	//recommend n=20;f1 is in [0,1]
	void HE6::generateAdLoadPF() {//the method to get Real PF is over sampling in 2 dimension
		int num = 1000;
		Real t = get_time();
		Real temp = 0.75*sin(0.5*OFEC_PI*t) + 1.25;
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0 + i * 1. / (num - 1);
			o_point[i][1] = (2 - sqrt(o_point[i][0]))*(1 - pow(o_point[i][0] / 2, temp));
			m_optima.append(o_point[i].vect());
		}
	}

	int HE6::evaluateObjective(Real *x, std::vector<Real> &obj) {//recommend the number of variables is 20
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real Ht = 0.75*std::sin(0.5 * OFEC_PI*t) + 1.25;

		for (size_t m = 0; m < m_num_objs; ++m)
		{
			if (m < 1) {
				obj[m] = 0;
				for (size_t i=2; i < m_num_vars; i += 2) {
					obj[m] += std::pow(x[i] - 0.8*x[0] * std::cos((6 * OFEC_PI*x[0] + (i + 1)*OFEC_PI / m_num_vars)/3), 2);
				}
				obj[m] = x[0] + 2. / (ceil(m_num_vars / 2.) - 1)*obj[m];
			}
			else {
				Real gt = 0;
				for (size_t i=1; i < m_num_vars; i += 2) {
					gt += std::pow(x[i] - 0.8*x[0] * std::cos((6 * OFEC_PI*x[0] + (i + 1)*OFEC_PI / m_num_vars) / 3), 2);
				}
				gt = 2 - std::sqrt(x[0]) + 2. / floor(m_num_vars / 2.)*gt;
				obj[m] = gt * (1 - std::pow(obj[0] / gt, Ht));
			}
		}
		return kNormalEval;
	}
}