#include "FDA3.h"

namespace ofec {
	FDA3::FDA3(param_map & v) : FDA3(v.at("problem name"), v.at("number of variables")) {

	}
	FDA3::FDA3(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void FDA3::initialize() {
		for (size_t i = 0; i < OFEC::problem::variable_size(); ++i) {//the x1 is in (0,1), others are in (-1,1)
			if (i < 5)//recommend the number of x1 is 5, x2 is 25
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

	//f1 =\sum(xi^Ft), f2 = g * h, f2 =(1+Gt)*(1 -sqrt(f1/(1+Gt))), f1 = [0, 5]
	void FDA3::generateAdLoadPF() {
		int num = 1000;
		Real t = get_time();
		Real temp = 1 + std::fabs(std::sin(0.5*OFEC_PI*t));
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0 + i * ceil(m_num_vars/6.) / (num-1);
			o_point[i][1] =temp*(1-std::sqrt(o_point[i][0]/temp));
			m_optima.append(o_point[i].vect());
		}
	}

	//f1 =\sum(xi^Ft), f2 = g * h, f2 =(1+Gt)*(1 -sqrt(f1/(1+Gt))), f1 = [0, 5]
	int FDA3::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real Gt = std::fabs(std::sin(0.5*OFEC_PI*t));
		Real Ft = std::pow(10, 2 * std::sin(0.5*OFEC_PI*t));
		Real gt = 1 + Gt;
		for (size_t i = ceil(m_num_vars / 6.); i < m_num_vars; ++i) {//variable group, ratio is 1:5
			gt += std::pow(x[i] - Gt, 2);
		}
		for (size_t m = 0; m < m_num_objs; ++m)
		{
			if (m < 1) {
				obj[m] = 0;
				for (size_t i = 0; i < ceil(m_num_vars / 6.); ++i) {
					obj[m] += std::pow(x[i], Ft);
				}
			}
			else
				obj[m] = gt * (1 - std::pow(obj[0] / gt, 1./2));
		}
		return kNormalEval;
	}
}