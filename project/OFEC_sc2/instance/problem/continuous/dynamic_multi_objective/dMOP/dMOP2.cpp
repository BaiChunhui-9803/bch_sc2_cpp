#include "dMOP2.h"

namespace ofec {
	dMOP2::dMOP2(param_map & v) : dMOP2(v.at("problem name"), v.at("number of variables")) {

	}
	dMOP2::dMOP2(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void dMOP2::initialize() {//all variables are in [0,1], recommend the number of x is 10
		setInitialDomain(0., 1.);
		setDomain(0., 1.);
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//f1=x1, f2=g*h=1-f1^Ht
	void dMOP2::generateAdLoadPF() {
		int num = 1000;
		Real t = get_time();
		Real Ht = 1.25 + 0.75*std::sin(0.5*OFEC_PI*t);
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0 + i * 1. / (num - 1);
			o_point[i][1] = 1 - std::pow(o_point[i][0], Ht);
			m_optima.append(o_point[i].vect());
		}
	}

	int dMOP2::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real Ht = 0.75*std::sin(0.5*OFEC_PI*t) + 1.25;
		Real Gt = std::sin(0.5*OFEC_PI*t);
		Real gt = 1;
		for (size_t i = 1; i < m_num_vars; ++i) {
			gt += std::pow(x[i] - Gt, 2);
		}
		for (size_t m = 0; m < m_num_objs; ++m) {
			if (m < 1)
				obj[m] = x[0];
			else
				obj[m] = gt * (1 - std::pow(obj[0] / gt, Ht));
		}
		return kNormalEval;
	}
}