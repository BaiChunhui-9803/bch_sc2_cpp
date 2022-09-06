#include "JY10.h"

namespace ofec {
	JY10::JY10(param_map & v) : JY10(v.at("problem name"), v.at("number of variables")) {

	}
	JY10::JY10(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void JY10::initialize() {
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

	void JY10::generateAdLoadPF() {
		int num = 2000;
		Real t = get_time();
		Real At = 0.05;
		Real Wt = 6;
		Real gt = 0; //g function
		Real Gt = fabs(sin(0.5*OFEC_PI*t));//change factor
		size_t rho_t = 5;//the time steps a type of problem lasts
		size_t pop_size= global::ms_arg.find("population size")->second;
		size_t R = ceil(3 * global::ms_global->m_uniform[caller::Problem]->next());
		size_t sigma = (m_effective_eval / (pop_size* get_duration_gen() * rho_t) + R) % 3;
		Real alpha_t = 1 + sigma * Gt;
		Real beta_t = alpha_t;
		std::vector<std::vector<Real>*> point(num);
		matrix o_point(num, 2);
		if (sigma == 0 || sigma == 1) {
			for (int i = 0; i < num; i++) {
				Real x = i * 1. / (num - 1);
				o_point[i][0] = pow((x + At * sin(Wt*OFEC_PI* x)), alpha_t);
				o_point[i][1] = pow((1 - x + At * sin(Wt*OFEC_PI* x)), beta_t);
				point[i] = &o_point[i].vect();
			}
		}
		else {
			for (int i = 0; i < num; i++) {
				Real x = i * 1. / (num - 1);
				o_point[i][0] = (1 - (m_num_vars - 1)*pow((1 - Gt), 2))*pow((x + At * sin(Wt*OFEC_PI * x)), alpha_t);
				o_point[i][1] = (1 - (m_num_vars - 1)*pow((1 - Gt), 2))*pow((1 - x + At * sin(Wt*OFEC_PI * x)), beta_t);
				point[i] = &o_point[i].vect();
			}
		}

		std::vector<std::vector<Real>*> &temp = get_nondominated_set(point, optimization_mode::Minimization);
		for (int i = 0; i < temp.size(); i++) {
			m_optima.append(*temp[i]);
		}
	}

	int JY10::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
		Real Gt = fabs(std::sin(0.5*OFEC_PI*t));//change factor
		size_t rho_t = 5;//the time steps a type of problem lasts
		Real At = 0.05;
		Real Wt = 6;
		Real R = 0;//random factor for the type of problem
		size_t sigma = 1;
		size_t pop_size = global::ms_arg.find("population size")->second;
		if (m_effective_eval % (pop_size * get_duration_gen()*rho_t)==0) {
			R = 3 * global::ms_global->m_uniform[caller::Problem]->next();
			sigma = (m_effective_eval /(pop_size * get_duration_gen()*rho_t) + (size_t)ceil(R)) % 3;
		}
		Real alpha_t = 1 + sigma * Gt;
		Real beta_t = alpha_t;
		for (size_t i = 1; i < global::ms_global->m_problem->variable_size(); ++i) {
			gt += pow(x[i] + sigma - Gt, 2);
		}
		for (size_t j = 0; j < global::ms_global->m_problem->objective_size(); ++j)
		{
			if (j < 1)
				obj[j] = (1 + gt)*pow(x[0] + At * std::sin(Wt*OFEC_PI*x[0]),alpha_t);
			else
				obj[j] = (1 + gt)*pow(1 - x[0] + At * std::sin(Wt*OFEC_PI*x[0]),beta_t);
		}
		return kNormalEval;
	}
}