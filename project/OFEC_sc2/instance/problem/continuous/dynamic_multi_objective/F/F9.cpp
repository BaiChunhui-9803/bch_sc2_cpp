#include "F9.h"

namespace ofec::DMOP {
	F9::F9(param_map & v) : F9(v.at("problem name"), v.at("number of variables")) {

	}
	F9::F9(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void F9::initialize() {
		for (size_t i = 0; i < OFEC::problem::variable_size(); ++i) {//all variables are in [0,5], recommend n=20;f1=[0,1]
			m_domain.setDomain(0., 5., i);
		}
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//f1=s^H, f2=(1-s)^H, f2=(1-f1^(1/H))^H
	void F9::generateAdLoadPF() {
		int num = 1000;
		Real t = get_time();
		Real temp1 = 1.25 + 0.75*std::sin(OFEC_PI*t);
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0 + i * 1. / (num - 1);
			o_point[i][1] = std::pow(1 - std::pow(o_point[i][0], (Real)1. / temp1), temp1);
			m_optima.append(o_point[i].vect());
		}
	}

	//f1=x1, f2=g*h
	int F9::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//��ֹ�����������ظ�����������ظ�����PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real a = 2 + 2 * std::cos(OFEC_PI*(t - std::floor(t)));
		Real b = 2 + 2 * std::sin(OFEC_PI*2*(t - std::floor(t)));
		Real Ht = 1.25 + 0.75*std::sin(OFEC_PI*t);

		for (size_t m = 0; m < m_num_objs; ++m)
		{
			if (m < 1) {
				obj[m] = std::pow(std::fabs(x[0] - a), Ht);
				for (size_t i = 0; i < m_num_vars; i += 2)
					obj[m] += std::pow(x[i] - b - 1 + std::pow(std::fabs(x[0] - a), Ht + (Real)(i + 1) / m_num_vars), 2);
			}
			else {
				obj[m] = std::pow(std::fabs(x[0] - a - 1), Ht);
				for (size_t i = 1; i < m_num_vars; i += 2)
					obj[m] += std::pow(x[i] - b - 1 + std::pow(std::fabs(x[0] - a), Ht + (Real)(i + 1) / m_num_vars), 2);
			}
		}
		return kNormalEval;
	}
}