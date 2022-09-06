#include "JY2.h"

namespace ofec {
	JY2::JY2(param_map & v) : JY2(v.at("problem name"), v.at("number of variables")) {

	}
	JY2::JY2(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void JY2::initialize() {
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

	void JY2::generateAdLoadPF() {
		int num = 2000;
		Real t = get_time();
		Real At = 0.05;
		Real Wt = floor(6 * sin(0.5*OFEC_PI*(t - 1)));
		std::vector<std::vector<Real>*> point(num);
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			Real x = i * 1. / (num - 1);
			o_point[i][0] = x + At * sin(Wt*OFEC_PI * x);
			o_point[i][1] = 1 - x + At * sin(Wt*OFEC_PI * x);
			point[i] = &o_point[i].vect();
		}

		std::vector<std::vector<Real>*> &temp = get_nondominated_set(point, optimization_mode::Minimization);
		for (int i = 0; i < temp.size(); i++) {
			m_optima.append(*temp[i]);
		}
	}

	int JY2::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
		Real Gt = std::sin(0.5*OFEC_PI*t);//change factor
		Real At = 0.05;
		Real Wt = std::floor(6*std::sin(0.5*OFEC_PI*(t-1)));
		for (size_t i = 1; i < global::ms_global->m_problem->variable_size(); ++i)
		{
			gt += std::pow(x[i] - Gt, 2);
		}
		for (size_t j = 0; j < global::ms_global->m_problem->objective_size(); ++j)
		{
			if (j < 1)
				obj[j] = (1 + gt)*(x[0] + At * std::sin(Wt*OFEC_PI*x[0]));
			else
				obj[j] = (1 + gt)*(1 - x[0] + At * std::sin(Wt*OFEC_PI*x[0]));
		}
		return kNormalEval;
	}
}