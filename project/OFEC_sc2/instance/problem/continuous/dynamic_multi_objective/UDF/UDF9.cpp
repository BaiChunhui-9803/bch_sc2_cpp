#include "UDF9.h"

namespace ofec {
	UDF9::UDF9(param_map & v) : UDF9(v.at("problem name"), v.at("number of variables")) {

	}
	UDF9::UDF9(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void UDF9::initialize() {
		for (size_t i = 0; i < m_num_vars; ++i) {//the first one is in [0,1], others are in [-2,2]
			if (i < 1)
				m_domain.setDomain(0., 1., i);
			else
				m_domain.setDomain(-1., 2., i);
		}
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//recommend n=20;
	void UDF9::generateAdLoadPF() {
		int num = 1000;
		Real Gt3 = sin(0.5*OFEC_PI*m_count[2]);;
		Real Ht4 = 0.5 + std::fabs(std::sin(0.5*OFEC_PI*m_count[3]));
		Real Ht5 = 0.5 + std::fabs(std::sin(0.5*OFEC_PI*m_count[4]));
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = std::fabs(Gt3) + i * 1. / (num - 1);
			o_point[i][1] = 1 + std::fabs(Gt3) - Ht4 * std::pow(o_point[i][0], Ht5);
			m_optima.append(o_point[i].vect());
		}
	}

	int UDF9::evaluateObjective(Real *x, std::vector<Real> &obj) {//recommend the number of variables is 20
		Real t = get_time();
		
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			Real temp = global::ms_global->m_uniform[caller::Problem]->next();
			if (temp >= 0 && temp <= 0.2)
				m_count[0]++;
			else if (temp > 0.2 && temp <= 0.4)
				m_count[1]++;
			else if (temp > 0.4 && temp <= 0.6)
				m_count[2]++;
			else if (temp > 0.6 && temp <= 0.8)
				m_count[3]++;
			else
				m_count[4]++;
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}

		for (size_t m = 0; m < m_num_objs; ++m)
		{
			if (m < 1) {
				obj[m] = 0;
				for (size_t i=2; i < m_num_vars; i += 2) {
					obj[m] += std::pow(x[i] - std::pow(x[0],0.5*(2+3.*(i-1)/(m_num_vars-2)\
						+ std::sin(0.5*OFEC_PI*m_count[1]))) - std::sin(0.5*OFEC_PI*m_count[2]), 2);
				}
				obj[m] = x[0] + std::fabs(std::sin(0.5*OFEC_PI*m_count[2])) + 2. / (ceil(m_num_vars / 2.) - 1)*obj[m];
			}
			else {
				obj[m] = 0;
				for (size_t i=1; i < m_num_vars; i += 2) {
					obj[m] += std::pow(x[i] - std::pow(x[0], 0.5*(2 + 3.*(i - 1) / (m_num_vars - 2)\
						+ std::sin(0.5*OFEC_PI*m_count[1]))) - std::sin(0.5*OFEC_PI*m_count[2]), 2);
				}
				obj[m] = 1 - (0.5 + std::fabs(std::sin(0.5*OFEC_PI*m_count[3])))*std::pow(x[0], 0.5 + \
					std::fabs(std::sin(0.5*OFEC_PI*m_count[4]))) + std::fabs(std::sin(0.5*OFEC_PI*m_count[2])) + 2. / (floor(m_num_vars / 2.))*obj[m];
			}
		}
		return kNormalEval;
	}
}