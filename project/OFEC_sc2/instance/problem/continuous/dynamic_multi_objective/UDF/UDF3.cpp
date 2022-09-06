#include "UDF3.h"

namespace ofec {
	UDF3::UDF3(param_map & v) : UDF3(v.at("problem name"), v.at("number of variables")) {

	}
	UDF3::UDF3(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void UDF3::initialize() {
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
	void UDF3::generateAdLoadPF() {
		int num = 10;
		Real t = get_time();
		Real Gt = sin(0.5*OFEC_PI*t);
		Real temp = std::fabs(Gt);
		matrix o_point(num, 2);//列放第一个坐标值
		for (int i = 0; i < num; i++) {
			o_point[i][0] = temp + (2*(i + 1)-1) / 2./num;
			o_point[i][1] = temp+(i+1)/(Real)num;
			m_optima.append(o_point[i].vect());
		}
	}

	int UDF3::evaluateObjective(Real *x, std::vector<Real> &obj) {//recommend the number of variables is 20
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real Gt = std::sin(0.5 * OFEC_PI*t);
		Real alpha = 0.1;
		size_t N = 10;
		Real temp = (1. / 2 / N + alpha)*(std::sin(2 * N*OFEC_PI*x[0]) - 2 * N*std::fabs(Gt));
		if (temp < 0)
			temp = 0;

		for (size_t m = 0; m < m_num_objs; ++m)
		{
			if (m < 1) {
				Real temp1 = 0,temp2=1;
				for (size_t i=2; i < m_num_vars; i += 2) {
					temp1 += 2*std::pow(x[i] - std::sin(6*OFEC_PI*x[0]+(i+1)*OFEC_PI/m_num_vars), 2);
					temp2 *= std::cos(20*OFEC_PI*(x[i] - std::sin(6 * OFEC_PI*x[0] + (i + 1)*OFEC_PI / m_num_vars))/std::sqrt(i+1));
				}
				obj[m] = x[0] + 2. / (ceil(m_num_vars / 2.) - 1)*std::pow(4*temp1-2*temp2+2,2) + temp;
			}
			else {
				Real temp1 = 0, temp2 = 1;
				for (size_t i=1; i < m_num_vars; i += 2) {
					temp1 += 2 * std::pow(x[i] - std::sin(6 * OFEC_PI*x[0] + (i + 1)*OFEC_PI / m_num_vars), 2);
					temp2 *= std::cos(20 * OFEC_PI*(x[i] - std::sin(6 * OFEC_PI*x[0] + (i + 1)*OFEC_PI / m_num_vars)) / std::sqrt(i + 1));
				}
				obj[m] = 1-x[0] + 2. / (floor(m_num_vars / 2.))*std::pow(4 * temp1 - 2 * temp2 + 2, 2) + temp;
			}
		}
		return kNormalEval;
	}
}