#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include"../instance/algorithm/combination/sequence/SP/SP_operator.h"
#include "../instance/algorithm/combination/gl/gl_adaptor_com.h"
#include "../instance/algorithm/combination/gl/gl_com.h"
#include "../instance/algorithm/combination/gl/gl_pop_seq.h"
#include"../instance/problem/combination/selectionProblem/selection_problem.h"
#include "../instance/algorithm/template/combination/multi_population/distance_calculator.h"
#include "../instance/algorithm/combination/gl/mp/gl_mp_pop.h"
using namespace ofec;
using namespace std;


TEST_CASE("com SP2", "[MP_distance_calculator]") {


	DistanceCalculator<SP_Interpreter> a;
}

TEST_CASE("com SP", "[SP]") {


	InstanceManager::ms_instance_manager.reset(new InstanceManager);

	SP::SelectionProblem s;
	Solution s1;

	ParamMap params;

	params["problem name"] = std::string("ComOP_DSP");
	params["numDim"] = "5";

	int id_rnd = ADD_RND(0.5);
	int id_param = ADD_PARAM(params);
	int id_pro = ADD_PRO(id_param, 0.1);

	GET_PRO(id_pro).initialize();

	

	using sol_type = OFEC::SP::SelectionProblem::solution_type;
	sol_type cur_sol(GET_PRO(id_pro).numObjectives(), GET_PRO(id_pro).numConstraints(), GET_PRO(id_pro).numVariables());
	int testnum(1);

	std::vector<Real> obj_val(0);
	Real mean_val(0), distri_val(0);
	int size(1000);
	while (testnum--) {
		cur_sol.initialize(id_pro, id_rnd);

		cout << "id\t" << testnum << "sol:\t";
		for (auto& it : cur_sol.variable().vect()) {
			cout << it << "\t";
		}

		for(int t(0);t<2000;++t)
		{

			cur_sol.evaluate(id_pro, -1, false);
			//cout << "obj:\t" << cur_sol.objective()[0] << endl;
			obj_val.clear();
			for (int idx(0); idx < size; ++idx) {
				cur_sol.evaluate(id_pro, -1, false);
				obj_val.push_back(cur_sol.objective()[0]);

			}
			mean_val = 0;
			distri_val = 0;
			for (auto& it : obj_val) mean_val += it;
			mean_val /= obj_val.size();
			for (auto& it : obj_val) distri_val += (it - mean_val) * (it - mean_val);
			distri_val = sqrt(distri_val / obj_val.size());
			cout << "mean obj:\t" << cur_sol.objective()[0] << "\t distri obj\t" << distri_val << endl;
			GET_DYN_SP(id_pro).change();
		}
		}



	std::vector<sol_type> samples;
	GET_DYN_SP(id_pro).generate_level_samples(samples, id_pro);
	for (auto& it : samples) {
		it.evaluate(id_pro, -1, false);
	}
	//GET_DYN_SP(id_pro).(samples, caller::Problem);
	std::vector<Real> mean(samples.size(), 0);
	std::vector<Real> var(samples.size(), 0);

	const int sample_num(1000);

	std::vector<Real> sample_val(sample_num);

	for (int indi(0); indi < samples.size(); ++indi) {
		for (int sam_id(0); sam_id < sample_num; ++sam_id) {
			samples[indi].evaluate(id_pro, -1, false);
			sample_val[sam_id] = samples[indi].objective()[0];
		}
		for (const auto& sample_iter : sample_val) {
			mean[indi] += sample_iter;
		}
		mean[indi] /= sample_val.size();
		for (int sam_id(0); sam_id < sample_num; ++sam_id) {
			var[indi] += (sample_val[sam_id] - mean[indi]) * ((sample_val[sam_id] - mean[indi]));
		}
		var[indi] /= sample_val.size();
		var[indi] = sqrt(var[indi]);
	}

	std::vector<Real> dis(samples.size(), 0);
	for (int indi(0); indi < samples.size(); ++indi) {
		for (auto& others : samples) {
			dis[indi] += samples[indi].variableDistance(others, id_pro);
		}
	}
	//s1.evaluate()
}




TEST_CASE("com alg", "[com][adaptor]") {
	

	InstanceManager::ms_instance_manager.reset(new InstanceManager);
	ParamMap params;
	params["problem name"] = std::string("ComOP_DSP");
	params["numDim"] = "5";

	params["algorithm name"] = std::string("GL_SP");
	params["changeFre"] = int(1e9);

	params["population size"] = int(40);
   // params["maximum evaluations"] = int(100000);
	//	params["number of variables"] = int();
	int id_param = ADD_PARAM(params);
	int id_pro = ADD_PRO(id_param, 0.1);
	GET_PRO(id_pro).initialize();
	int id_alg = ADD_ALG(id_param, 0.1, 0.01, -1);
	GET_ALG(id_alg).initialize();
	GET_ALG(id_alg).run();

	DEL_ALG(id_alg);
	DEL_PRO(id_pro);
	DEL_PARAM(id_param);

}


//#include <torch/torch.h>
//
//TEST_CASE("prediction", "[lstm][calbest]")
//{
//
//	torch::Tensor m_input;
//
//
//}