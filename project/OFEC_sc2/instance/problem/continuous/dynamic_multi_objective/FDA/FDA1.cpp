#include "FDA1.h"

#ifdef OFEC_DEMO
#include "../../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace ofec {
	FDA1::FDA1(param_map & v) : FDA1(v.at("problem name"), v.at("number of variables")) {

	}
	FDA1::FDA1(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {

	}

	void FDA1::initialize() {
		for (size_t i = 0; i < OFEC::problem::variable_size(); ++i) {//the first variable is in [0,1], others are in [-1,1]
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

	//f1 = x1, f2 = g * h, f2 = 1 - sqrt(f1), f1 = [0, 1]
	void FDA1::generateAdLoadPF(){
		int num = 1000;
		matrix o_point(num,2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0+i*1./(num-1);
			o_point[i][1] = 1 - std::sqrt(o_point[i][0]);
			m_optima.append(o_point[i].vect());
		}
	}

	//f1=x1, f2=g*h, f2=1-sqrt(f1), f1=[0,1]
	int FDA1::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
#ifdef OFEC_DEMO
			Demo::msp_buffer->updateFitnessLandsacpe_();
#endif
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real gt = 1; //g function
		Real Gt = std::sin(0.5*OFEC_PI*t);//change factor
		for (size_t i = 1; i < m_num_vars; ++i)
		{
			gt += std::pow(x[i] - Gt, 2);
		}
		for (size_t j = 0; j < m_num_objs; ++j)
		{
			if (j < 1)
				obj[j] = x[j];
			else
				obj[j] = gt * (1 - std::sqrt(obj[0] / gt));
		}
		//std::cout << obj[0] << "  " << obj[1] << std::endl;
		return kNormalEval;
	}
}