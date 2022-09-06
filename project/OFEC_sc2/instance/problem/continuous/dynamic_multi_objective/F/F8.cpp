#include "F8.h"

namespace ofec::DMOP {
	F8::F8(param_map & v) : F8(v.at("problem name"), v.at("number of variables")) {

	}
	F8::F8(const std::string & name, size_t size_var) : problem(name, size_var, 3), DMOPs(name, size_var, 3) {

	}

	void F8::initialize() {
		for (size_t i = 0; i < m_num_vars; ++i) {//the first two is in[0,1]��others are in [-1,2], recommend n=20
			if (i < 2)
				m_domain.setDomain(0., 1., i);
			else
				m_domain.setDomain(-1., 2.,i);
		}
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//f1^2+f2^2+f3^2=1
	void F8::generateAdLoadPF() {
		int n = 100;//the number of sample points in each dimension
		int num = n*(1+n)/2;//total number of sample points in Real PF
		matrix o_point(num, 3);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < i + 1; j++) {
				o_point[i*(1 + i) / 2+j][0] =1-(Real)i/(n-1) ;
				o_point[i*(1 + i) / 2+j][1] = (Real)i*j / (n - 1);
				o_point[i*(1 + i) / 2+j][2] =1- o_point[i*(1 + i) / 2+j][0]- o_point[i*(1 + i) / 2+j][1];
			}
		}
		//map points on x+y+z=1 into f1^2+f2^2+f3^2=1
		for (int i = 0; i < num; i++) {
			Real temp = sqrt(pow(o_point[i][0],2)+pow(o_point[i][1], 2)+pow(o_point[i][2], 2));
			o_point[i][0] = o_point[i][0] / temp;
			o_point[i][1] = o_point[i][1] / temp;
			o_point[i][2] = o_point[i][2] / temp;
			m_optima.append(o_point[i].vect());
		}
	}

	//f1=x1, f2=g*h
	int F8::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real t = get_time();
		Real Gt = std::sin(0.5*OFEC_PI*t);
		Real Ht = 1.25 + 0.75*std::sin(OFEC_PI*t);
		Real g = 0;
		for (size_t i = 2; i < m_num_vars; i++) {
			g += std::pow(x[i] - std::pow((x[0] + x[1]) / 2, Ht) - Gt, 2);
		}

		obj[0] = (1 + g)*std::cos(0.5*OFEC_PI*x[1])*std::cos(0.5*OFEC_PI*x[0]);
		obj[1] = (1 + g)*std::cos(0.5*OFEC_PI*x[1])*std::sin(0.5*OFEC_PI*x[0]);
		obj[2] = (1 + g)*std::sin(0.5*OFEC_PI*x[1]);
		return kNormalEval;
	}
}