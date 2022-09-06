#include "UDF7.h"

namespace ofec {
	UDF7::UDF7(param_map & v) : UDF7(v.at("problem name"), v.at("number of variables")) {

	}
	UDF7::UDF7(const std::string & name, size_t size_var) : problem(name, size_var, 3), DMOPs(name, size_var, 3) {

	}

	void UDF7::initialize() {
		for (size_t i = 0; i < m_num_vars; ++i) {//the first two is in [0,1], others are in [-2,2]
			if (i < 2)
				m_domain.setDomain(0., 1., i);
			else
				m_domain.setDomain(-2., 2., i);
		}
		
		//set_duration_gen(global::ms_arg.find("durationGeneration")->second);
		set_change_severity(global::ms_arg.find("changeSeverity")->second);
		set_change_fre(global::ms_arg.find("changeFre")->second);

		generateAdLoadPF();
		m_initialized = true;
	}

	//(f1-Gt)^2+(f2-Gt)^2+(f3-Gt)^2=Rt^2, recommend n=20;
	void UDF7::generateAdLoadPF() {
		int n = 100;//the number of sample points in each dimension
		int num = n * (1 + n) / 2;
		Real t = get_time();
		Real Gt = sin(0.5*OFEC_PI*t);
		Real Rt = 1+std::fabs(Gt);
		matrix o_point(num, 3);
		for (int i = 0; i < n; i++) {//get uniform points in x+y+z=1
			for (int j = 0; j < i + 1; j++) {
				o_point[i*(1 + i) / 2 + j][0] = Rt+Gt - Rt*i / (n - 1);
				o_point[i*(1 + i) / 2 + j][1] = Gt+ Rt * i*j / (n - 1);
				o_point[i*(1 + i) / 2 + j][2] = 1 +fabs(Gt)+3*Gt- o_point[i*(1 + i) / 2 + j][0] - o_point[i*(1 + i) / 2 + j][1];
			}
		}
		//带有球心偏置和半径放缩的球面
		//将x+y+z=1+|Gt|+3*Gt平面上均匀分布点映射到(f1-Gt)^2+(f2-Gt)^2+(f3-Gt)^2=(1+Gt)^2的圆上
		for (int i = 0; i < num; i++) {
			Real temp = sqrt(pow(o_point[i][0]-Gt, 2) + pow(o_point[i][1]-Gt, 2) + pow(o_point[i][2]-Gt, 2));
			o_point[i][0] = Rt * (o_point[i][0] - Gt) / temp + Gt;
			o_point[i][1] = Rt * (o_point[i][1] - Gt) / temp + Gt;
			o_point[i][2] = Rt * (o_point[i][2] - Gt) / temp + Gt;
			m_optima.append(o_point[i].vect());
		}
	}

	int UDF7::evaluateObjective(Real *x, std::vector<Real> &obj) {//recommend the number of variables is 20
		Real t = get_time();
		if (time_changed() && t != 0. && (!get_updated_state())) {//防止不计数评价重复更新问题和重复采样PF
			m_optima.clear();
			generateAdLoadPF();
			set_updated_state(true);
		}
		else if (m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second != 0) {
			set_updated_state(false);
		}
		Real Gt = std::sin(0.5 * OFEC_PI*t);//Vertical or Horizontal shift in PF or PS
		Real Rt = 1 + std::fabs(Gt);//Radius variation in three dimensional PF

		for (size_t m = 0; m < m_num_objs; ++m)
		{
			if (m =0) {
				obj[m] = 0;
				for (size_t i=3; i < m_num_vars; i += 3) {
					obj[m] += std::pow(x[i] - 2*x[1]*std::sin(2*OFEC_PI*x[0]+(i+1)*OFEC_PI/m_num_vars), 2);
				}
				obj[m] = Rt*std::cos(0.5*OFEC_PI*x[0])*std::cos(0.5*OFEC_PI*x[1])+ 2. / (ceil(m_num_vars / 3.) - 1)*obj[m]+Gt;
			}
			else if(m=1){
				obj[m] = 0;
				for (size_t i=4; i < m_num_vars; i += 3) {
					obj[m] += std::pow(x[i] - 2 * x[1] * std::sin(2 * OFEC_PI*x[0] + (i + 1)*OFEC_PI / m_num_vars), 2);
				}
				obj[m] = Rt * std::cos(0.5*OFEC_PI*x[0])*std::sin(0.5*OFEC_PI*x[1]) + 2. / (ceil((m_num_vars+1) / 3.) - 1)*obj[m] + Gt;
			}
			else {
				obj[m] = 0;
				for (size_t i=2; i < m_num_vars; i += 3) {
					obj[m] += std::pow(x[i] - 2 * x[1] * std::sin(2 * OFEC_PI*x[0] + (i + 1)*OFEC_PI / m_num_vars), 2);
				}
				obj[m] = Rt * std::sin(0.5*OFEC_PI*x[0]) + 2. / (ceil((m_num_vars-1) / 3.) - 1)*obj[m] + Gt;
			}
		}
		return kNormalEval;
	}
}