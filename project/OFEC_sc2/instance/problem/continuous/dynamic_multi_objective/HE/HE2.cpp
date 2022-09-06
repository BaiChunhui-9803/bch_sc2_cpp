#include "HE2.h"
#include "../../../../../utility/functional.h"

namespace ofec {
	HE2::HE2(param_map & v) : HE2(v.at("problem name"), v.at("number of variables")) {

	}
	HE2::HE2(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void HE2::initialize() {
		for (size_t i = 0; i < OFEC::problem::variable_size(); ++i) {//all variable is in (0,1)
			m_domain.setDomain(0., 1., i);
		}
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//f1 = x1; f2 = g * h; f2 = 1 - (f1)^Ht-f1^Ht*sin(10πtf1); f1 = [0, 1]
	void HE2::generateAdLoadPF() {
		int num = 2000;
		Real t = get_time();
		Real temp1 = 0.75*std::sin(0.5*OFEC_PI*t) + 1.25;
		std::vector<std::vector<Real>*> point(num);
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0 + i * 1. / (num - 1);
			o_point[i][1] = 1 - pow(sqrt(o_point[i][0]), temp1) - pow(o_point[1][0], temp1)*sin(10 * OFEC_PI*t*o_point[1][0]);
			point[i] = &o_point[i].vect();
		}

		std::vector<std::vector<Real>*> &temp = get_nondominated_set(point, optimization_mode::Minimization);
		for (int i = 0; i < temp.size(); i++) {
			m_optima.append(*temp[i]);
		}
	}

	//f1 = x1; f2 = g * h; f2 = 1 - (f1)^Ht-f1^Ht*sin(10πtf1)
	int HE2::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
		Real Ht = 0.75*std::sin(0.5*OFEC_PI*t) + 1.25;
		for (size_t i = 1; i < m_num_vars; ++i)
		{
			gt += x[i];
		}
		gt = 1 + 9 * gt / (m_num_vars - 1);
		for (size_t j = 0; j < m_num_objs; ++j)
		{
			if (j < 1)
				obj[j] = x[j];
			else
				obj[j] = gt * (1 - std::pow(obj[0] / gt,Ht) - std::pow(obj[0] / gt,Ht) * std::sin(10 * OFEC_PI*t*obj[0]));
		}
		return kNormalEval;
	}
}