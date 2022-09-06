#include "../utility/catch.hpp"
#include "../core/instance_manager.h"
#include "../instance/problem/realworld/csiwdn/csiwdn.h"
#include <fstream>

using namespace ofec;

TEST_CASE("CSIWDN", "[CSIWDN]") 
{
	InstanceManager::ms_instance_manager.reset(new InstanceManager);
	ParamMap params;
	params["problem name"] = std::string("CSIWDN");
	params["dataFile1"] = std::string("Net2_97_case2_dynamic_157");
	params["dataFile2"] = std::string("Net2");

	SECTION("CASE EPANET") {
		int id_param = ADD_PARAM(params);
		int id_pro = ADD_PRO(id_param, 0.1);
		GET_PRO(id_pro).initialize();
	}

	//SECTION("CASE LSTM") {
	//	int id_param = ADD_PARAM(params);
	//	int id_pro = ADD_PRO(id_param, 0.1);
	//	GET_PRO(id_pro).initialize();
	//	GET_CSIWDN(id_pro).setUseLSTM(true);

	//	Solution<VarCSIWDN> sol(1, 0, GET_CSIWDN(id_pro).numberSource());

	//	int id_rnd = ADD_RND(0.5);
	//	sol.initialize(id_pro, id_rnd);
	//	sol.evaluate(id_pro, -1);

	//	GET_CSIWDN(id_pro).setAlgorithmStart();
	//	sol.initialize(id_pro, id_rnd);
	//	for (size_t i = 0; i < 10000; ++i)
	//		sol.evaluate(id_pro, -1);

	//	DEL_PRO(id_pro);
	//}

	//SECTION("CASE algorithm") {
	//	
	//	params["algorithm name"] = std::string("AMP-GL-SaDE");
	//	params["subpopulation size"] = 20;
	//	params["number of SubPopulations"] = 10;
	//	params["alpha"] = 2;
	//	params["beta"] = 2;
	//	params["maximum evaluations"] = 200000;

	//	int id_param = ADD_PARAM(params);

	//	int id_pro = ADD_PRO(id_param, 0.1);
	//	int id_alg = ADD_ALG(id_param, id_pro, 0.01, -1);

	//	GET_PRO(id_pro).initialize();
	//	GET_ALG(id_alg).initialize();
	//	GET_ALG(id_alg).run();


	//	DEL_ALG(id_alg);
	//	DEL_PRO(id_pro);
	//	DEL_PARAM(id_param);
	//}
}
