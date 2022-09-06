#include "JY3.h"

namespace ofec {
	JY3::JY3(param_map & v) : JY3(v.at("problem name"), v.at("number of variables")) {

	}
	JY3::JY3(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void JY3::initialize() {
		for (size_t i = 0; i < OFEC::problem::variable_size(); ++i) {//the first variable is in (0,1), others are in (-1,1)
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

	void JY3::generateAdLoadPF() {
		int num = 2000;
		Real t = get_time();
		Real At = 0.05;
		Real Wt = floor(6 * sin(0.5*OFEC_PI*(t - 1)));
		Real alpha_t = floor(100 * pow(sin(0.5*OFEC_PI*t), 2));
		std::vector<std::vector<Real>*> point(num);
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			Real y = fabs(i * 1. / (num - 1)*sin((2 * alpha_t + 0.5)*OFEC_PI*i * 1. / (num - 1)));
			o_point[i][0] = y + At * sin(Wt*OFEC_PI*y);
			o_point[i][1] = 1 - y + At * sin(Wt*OFEC_PI*y);
			point[i] = &o_point[i].vect();
		}

		std::vector<std::vector<Real>*> &temp = get_nondominated_set(point, optimization_mode::Minimization);
		for (int i = 0; i < temp.size(); i++) {
			m_optima.append(*temp[i]);
		}
	}

	int JY3::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real gt = 0; //g function
		Real Gt = sin(0.5*OFEC_PI*t);//change factor
		Real At = 0.05;
		Real alpha_t = floor(100 * std::pow(sin(0.5*OFEC_PI*t),2));
		Real Wt = floor(6 * sin(0.5*OFEC_PI*(t - 1)));
		Real y1 = fabs(x[0]*sin((2*alpha_t+0.5)*OFEC_PI*x[0]));
		for (size_t i = 1; i < global::ms_global->m_problem->variable_size(); ++i){
			if (i < 2)
				gt += pow(pow(x[i], 2) - y1, 2);
			else
				gt += pow(pow(x[i],2)-x[i-1],2);
		}
		for (size_t j = 0; j < global::ms_global->m_problem->objective_size(); ++j)
		{
			if (j < 1)
				obj[j] = (1 + gt)*(y1 + At * std::sin(Wt*OFEC_PI*y1));
			else
				obj[j] = (1 + gt)*(1 - y1 + At * std::sin(Wt*OFEC_PI*y1));
		}
		return kNormalEval;
	}
}