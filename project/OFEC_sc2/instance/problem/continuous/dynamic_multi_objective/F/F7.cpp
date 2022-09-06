#include "F7.h"

namespace ofec::DMOP {
	F7::F7(param_map & v) : F7(v.at("problem name"), v.at("number of variables")) {

	}
	F7::F7(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void F7::initialize() {
		for (size_t i = 0; i < OFEC::problem::variable_size(); ++i) {//all variable is in [0,5], recommend n=20
			m_domain.setDomain(0., 5., i);
		}

		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//f1=s^H, f2=(1-s)^H, f1=[0,1], f2=(1-f1^(1/H))^H
	void F7::generateAdLoadPF() {
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
	int F7::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//��ֹ�����������ظ�����������ظ�����PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real a = 3.4 + 1.7 * std::sin(OFEC_PI*t)*(1 - std::sin(OFEC_PI*t));
		Real b = 2.1 + 1.4 * std::cos(OFEC_PI*t)*(1 - std::sin(OFEC_PI*t));
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