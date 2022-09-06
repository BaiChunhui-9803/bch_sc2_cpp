#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include "../core/algorithm/population.h"
#include "../core/algorithm/individual.h"

#include "../instance/algorithm/combination/set_com_alg.h"

#include<string>
#include<iostream>

#include"../instance/problem/combination/selectionProblem/selection_problem.h"
using namespace ofec;
using namespace std;



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
	sol_type cur_sol(GET_PRO(id_pro).numObjectives(),GET_PRO(id_pro).numConstraints(), GET_PRO(id_pro).numVariables());
	int testnum(1000);

	std::vector<Real> obj_val(0);
	Real mean_val(0), distri_val(0);
	int size(1000);
	while (testnum--) {
		cur_sol.initialize(id_pro, id_rnd);
		
		cout << "id\t" << testnum << "sol:\t";
		for (auto& it : cur_sol.variable().vect()) {
			cout << it << "\t";
		}
		cur_sol.evaluate(id_pro,-1,false);
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
	}



	std::vector<sol_type> samples;
	GET_DYN_SP(id_pro).generate_level_samples(samples,id_pro);
	for (auto& it : samples) {
		it.evaluate(id_pro, -1,false);
	}
	//GET_DYN_SP(id_pro).(samples, caller::Problem);
	std::vector<Real> mean(samples.size(), 0);
	std::vector<Real> var(samples.size(), 0);

	const int sample_num(1000);

	std::vector<Real> sample_val(sample_num);

	for (int indi(0); indi < samples.size(); ++indi) {
		for (int sam_id(0); sam_id < sample_num; ++sam_id) {
			samples[indi].evaluate(id_pro,-1,false);
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
			dis[indi] += samples[indi].variableDistance(others,id_pro);
		}
	}



	//s1.evaluate()
}



TEST_CASE("combinatorial optimization", "[TSP][ACO]") {
	InstanceManager::ms_instance_manager.reset(new InstanceManager);

	ParamMap params;
	params["problem name"] = std::string("TSP");
	params["dataFile1"] = std::string("ATT48");

	params["algorithm name"] = std::string("MMAS_TSP");
	//		params["algorithm name"] = std::string("AS_TSP");

	params["population size"] = int(40);
	params["maximum evaluations"] = int(5000);
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


TEST_CASE("com SP algorithm", "[SP][AS]") {
	InstanceManager::ms_instance_manager.reset(new InstanceManager);

		ParamMap params;
		params["problem name"] = std::string("ComOP_DSP");
		params["numDim"] = "5";

		params["algorithm name"] = std::string("AS_SP");
		params["changeFre"] = int(100000);

		params["population size"] = int(40);
	//	params["maximum evaluations"] = int(5000);
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


//#include<Eigen>
//#include <iostream>
//#include <mathtoolbox/rbf-interpolation.hpp>
//#include <random>
//#include <timer.hpp>
//#include <vector>
//
//using Eigen::MatrixXd;
//using Eigen::Vector2d;
//using Eigen::VectorXd;
//
//namespace
//{
//	std::random_device                     seed;
//	std::default_random_engine             engine(seed());
//	std::uniform_real_distribution<double> uniform_dist(-1.0, 1.0);
//	 ±º‰–Ú¡–‘§≤‚
//		double CalcFunction(const Vector2d& x) { return std::sin(10.0 * x(0)) + std::sin(10.0 * x(1)); }
//
//	std::shared_ptr<timer::Timer> timer_object;
//} // namespace
//
//void PerformTest()
//{
//	constexpr int    number_of_samples = 500;
//	constexpr int    number_of_test_samples = 100;
//	constexpr double noise_intensity = 0.1;
//
//	// Generate scattered data (in this case, 500 data points in a 2-dimensional space)
//	Eigen::MatrixXd X = Eigen::MatrixXd::Random(2, number_of_samples);
//	Eigen::VectorXd y(number_of_samples);
//	for (int i = 0; i < number_of_samples; ++i)
//	{
//		y(i) = CalcFunction(X.col(i)) + noise_intensity * uniform_dist(engine);
//	}
//
//	// Define interpolation settings
//	const auto     kernel = mathtoolbox::ThinPlateSplineRbfKernel();
//	constexpr bool use_regularization = true;
//
//	// Instantiate an interpolator
//	mathtoolbox::RbfInterpolator rbf_interpolator(kernel);
//
//	// Set data
//	rbf_interpolator.SetData(X, y);
//
//	// Calculate internal weights with or without regularization
//	timer_object = std::make_shared<timer::Timer>("CalcWeights()");
//	rbf_interpolator.CalcWeights(use_regularization);
//	timer_object = nullptr;
//
//	// Calculate interpolated values on randomly sampled points
//	Eigen::MatrixXd X_test = Eigen::MatrixXd::Random(2, number_of_test_samples);
//	Eigen::VectorXd y_test(number_of_test_samples);
//	timer_object = std::make_shared<timer::Timer>("CalcValue() * " + std::to_string(number_of_test_samples));
//	for (int i = 0; i < number_of_test_samples; ++i)
//	{
//		y_test(i) = rbf_interpolator.CalcValue(X_test.col(i));
//	}
//	timer_object = nullptr;
//
//	// Display the results in the CSV format
//	std::cout << "x(0),x(1),y" << std::endl;
//	for (int i = 0; i < number_of_test_samples; ++i)
//	{
//		std::cout << X_test(0, i) << "," << X_test(1, i) << "," << y_test(i) << std::endl;
//	}
//}
//
//int main()
//{
//	PerformTest();
//
//	return 0;
//}
