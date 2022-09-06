#include "dMOP3.h"

namespace ofec {
	dMOP3::dMOP3(param_map & v) : dMOP3(v.at("problem name"), v.at("number of variables")) {

	}
	dMOP3::dMOP3(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void dMOP3::initialize() {//all variables are in [0,1], recommend the number of x is 10
		setInitialDomain(0., 1.);
		setDomain(0., 1.);
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//f1=x1, f2=g*h=1-f1^(1/2)
	void dMOP3::generateAdLoadPF() {
		int num = 1000;
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0 + i * 1. / (num - 1);
			o_point[i][1] = 1 - std::pow(o_point[i][0], (Real)0.5);
			m_optima.append(o_point[i].vect());
		}
	}

	int dMOP3::evaluateObjective(Real *x, std::vector<Real> &obj) {
		size_t r = std::ceil(m_num_vars * global::ms_global->m_uniform[caller::Problem]->next());
		Real t = get_time();
		Real Ht = 0.75*std::sin(0.5*OFEC_PI*t) + 1.25;
		Real Gt = std::sin(0.5*OFEC_PI*t);
		Real gt = 1;

		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			size_t r = std::ceil(10 * global::ms_global->m_uniform[caller::Problem]->next());//choose one dimension as the first objective randomly
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}

	    for (size_t i = 0; i < m_num_vars; ++i) {
			if (i != r - 1)
				gt += std::pow(x[i] - Gt, 2);
		}
		for (size_t m = 0; m < m_num_objs; ++m) {
			if (m < 1)
				obj[m] = x[r - 1];
			else
				obj[m] = gt * (1 - std::sqrt(obj[0] / gt));
		}
		return kNormalEval;
	}
}