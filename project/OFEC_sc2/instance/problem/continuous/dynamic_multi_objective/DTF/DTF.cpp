#include "DTF.h"

namespace ofec {
	DTF::DTF(const ParamMap &v) :DTF(v.at("problem name"), v.at("number of variables")) {

	}

	DTF::DTF(const std::string & name, size_t size_var) : problem(name, size_var, 2), DMOPs(name, size_var, 2) {
		
	}

	void DTF::initialize() {
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

	void DTF::generateAdLoadPF() {
		int num = 2000;
		Real t = get_time();
		Real temp1 = 0.2 + 4.8*t*t;
		size_t s = 3;
		Real temp2 = t * s;
		std::vector<std::vector<Real>*> point(num);
		matrix o_point(num, 2);
		for (int i = 0; i < num; i++) {
			o_point[i][0] = 0 + i * 1. / (num - 1);
			o_point[i][1] = 2 - std::pow(o_point[i][0], temp1) - o_point[i][0] * std::pow(std::fabs(std::sin(temp2*OFEC_PI*o_point[i][0])), temp1);//目标关系
			point[i] = &o_point[i].vect();
		}
		
		std::vector<std::vector<Real>*> &temp = get_nondominated_set(point, optimization_mode::Minimization);
		for (int i = 0; i < temp.size(); i++) {
			m_optima.append(*temp[i]);
		}
		//std::vector<int> m_rank(num);
		//std::vector<optimization_mode> opt_mode({optimization_mode::Minimization,optimization_mode::Minimization });
		////NS::deductive_sort(point, m_rank, opt_mode);
		//int lin=NS::fast_sort(point,m_rank,opt_mode);//返回排序层数
		/*std::ofstream out("../../../../../../DTF.txt",std::ios::app);
		if (out) {
			out << "obj1" << " " << "obj2" << std::endl;
			for (int i = 0; i < temp.size(); i++) {
				out << (*point[i])[0] << " " << (*point[i])[1] << std::endl;
			}
			out.close();
		}*/
	}

	//f1=x1, f2=g*h, recommend the number of decision variable is 20
	int DTF::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		
		Real gt = 1;
		Real alpha_t = 0.2+4.8*t*t;
		Real beta_t = std::pow(10,2*std::sin(0.5*OFEC_PI*t));
		Real gamma_t = std::sin(0.5*OFEC_PI*t);
		size_t s = 3;
		Real pha_t = s * t;//the number of sections of the PF
		Real omega_t = 0.4*pha_t;
		for (size_t i = 1; i < m_num_vars; ++i){
			gt += std::pow(x[i] - gamma_t, 2)-std::cos(omega_t*OFEC_PI*(x[i]-gamma_t))+1;
		}
		for (size_t m = 0; m < m_num_objs; ++m){
			if (m < 1)
				obj[m] = std::pow(x[0],beta_t);
			else
				obj[m] = gt * (2 - std::pow(obj[0] / gt,alpha_t)- obj[0] / gt*std::pow(std::fabs(std::sin(pha_t*OFEC_PI*obj[0])),alpha_t));
		}
		return kNormalEval;
	}
}