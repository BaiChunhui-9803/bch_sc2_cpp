#include "FDA5.h"

namespace ofec {
	FDA5::FDA5(param_map & v) : FDA5(v.at("problem name"), v.at("number of variables")) {

	}
	FDA5::FDA5(const std::string & name, size_t size_var) : problem(name, size_var, 3), DMOPs(name, size_var, 3) {

	}

	void FDA5::initialize() {
		for (size_t i = 0; i < OFEC::problem::variable_size(); ++i) {
			m_domain.setDomain(0., 1., i);
		}
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//f1^2+f2^2+f3^2=(1+Gt)^2, recommend n=20
	void FDA5::generateAdLoadPF() {
		int n = 100;
		int num = n * (1 + n) / 2;
		Real t = get_time();
		Real Gt = fabs(sin(0.5*OFEC_PI*t));
		matrix o_point(num, 3);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < i + 1; j++) {
				o_point[i*(1 + i) / 2 + j][0] = (1 + Gt)*(1 - (Real)i / (n - 1));
				o_point[i*(1 + i) / 2 + j][1] = (1 + Gt)*(Real)i*j / (n - 1);
				o_point[i*(1 + i) / 2 + j][2] = 1 - o_point[i*(1 + i) / 2 + j][0] - o_point[i*(1 + i) / 2 + j][1];
			}
		}
		
		for (int i = 0; i < num; i++) {
			Real temp = sqrt(pow(o_point[i][0], 2) + pow(o_point[i][1], 2) + pow(o_point[i][2], 2));
			o_point[i][0] = (1 + Gt)*o_point[i][0] / temp;
			o_point[i][1] = (1 + Gt)*o_point[i][1] / temp;
			o_point[i][2] = (1 + Gt)*o_point[i][2] / temp;
			m_optima.append(o_point[i].vect());
		}
	}

	//map x to y, f1^2+f2^2+...+fn^2=(1+Gt)^2, f = [0, 12],
	int FDA5::evaluateObjective(Real *x, std::vector<Real> &obj) {//recommend n=M+9, x2 has 10 variables
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
		Real Ft = 1 + 100 * std::pow(std::sin(0.5*OFEC_PI*t), 4);
		Real gt = Gt;
		for (size_t i = m_num_vars -m_num_objs+1; i < m_num_vars; ++i) {
			gt += std::pow(x[i] - Gt, 2);
		}
		for (size_t m = 0; m < m_num_objs; ++m)
		{
			obj[m] = 1 + gt;
			if (m < m_num_objs - 1)
			{
				size_t M = 0;
				for (size_t j=0; j < m_num_objs - m - 1; ++j,++M)
					obj[m] *= std::cos(0.5 * pow(x[M], Ft) * OFEC_PI);
				if (m > 0)
					obj[m] = obj[m] * sin(0.5*std::pow(x[M + 1], Ft) * OFEC_PI);
			}
			else
				obj[m] = obj[m] * sin(0.5*std::pow(x[0], Ft) * OFEC_PI);
		}
		return kNormalEval;
	}
}