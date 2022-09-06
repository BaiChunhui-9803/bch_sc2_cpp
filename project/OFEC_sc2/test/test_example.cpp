#include "../utility/catch.hpp"
#include"../core/global.h"
#include "../core/instance_manager.h"
#include<iostream>
#include "../core/algorithm/solution.h"
#include "../instance/algorithm/template/classic/de/de_pop.h"
using namespace std;
using namespace ofec;

// this is a


TEST_CASE("exmaple", "[case1]")
{
	InstanceManager::ms_instance_manager.reset(new InstanceManager);

	ParamMap params;
	params["problem name"] = std::string("BBOB_F01");
	params["number of variables"] = 2;

	SECTION("CASE problem") {
		int id_param = ADD_PARAM(params);
		int id_pro = ADD_PRO(id_param, 0.1);
		GET_PRO(id_pro).initialize();
		DEL_PRO(id_pro);
	}

	SECTION("CASE solution") {
		int id_param = ADD_PARAM(params);
		int id_pro = ADD_PRO(id_param, 0.1);
		GET_PRO(id_pro).initialize();


		Solution<> sol(GET_PRO(id_pro).numObjectives(), GET_PRO(id_pro).numConstraints(), GET_CONOP(id_pro).numVariables());

		int id_rnd = ADD_RND(0.1);

		sol.initialize(id_pro, id_rnd);
		sol.evaluate(id_pro, -1);

		DEL_RND(id_rnd);
		DEL_PRO(id_pro);
		DEL_PARAM(id_param);
	}


	SECTION("CASE population") {
		int id_param = ADD_PARAM(params);
		int id_pro = ADD_PRO(id_param, 0.1);
		GET_PRO(id_pro).initialize();

		PopDE<> pop_de(20,id_pro);

		int id_rnd = ADD_RND(0.1);

		pop_de.initialize(id_pro, id_rnd);
		pop_de.evaluate(id_pro, -1);
		pop_de.evolve(id_pro, -1, id_rnd);

		DEL_RND(id_rnd);
		DEL_PRO(id_pro);
		DEL_PARAM(id_param);
	}

	SECTION("CASE algorithm") {
		InstanceManager::ms_instance_manager.reset(new InstanceManager);

		ParamMap params;
		params["problem name"] = std::string("BBOB_F01");
		params["number of variables"] = 2;
		params["algorithm name"] = std::string("Canonical-DE");
		params["population size"] = 20;
		params["maximum evaluations"] = 1000;

		int id_param = ADD_PARAM(params);

		int id_pro = ADD_PRO(id_param, 0.1);
		int id_alg = ADD_ALG(id_param, id_pro, 0.01, -1);

		GET_PRO(id_pro).initialize();
		GET_ALG(id_alg).initialize();
		GET_ALG(id_alg).run();


		DEL_ALG(id_alg);
		DEL_PRO(id_pro);
		DEL_PARAM(id_param);
	}

}




void runAlgExample(int id_alg) {
	{
		std::unique_lock<std::mutex> lock(g_cout_mutex);
		std::cout << "------------ Start run: Alg ID " << id_alg << std::endl;
	}
	GET_ALG(id_alg).run();
	{
		std::unique_lock<std::mutex> lock(g_cout_mutex);
		std::cout << "++++++++++++ Finish run: Alg ID " << id_alg << std::endl;
	}
}



TEST_CASE("example2", "[case2]")
{


	InstanceManager::ms_instance_manager.reset(new InstanceManager);

	ParamMap params;
	params["problem name"] = std::string("BBOB_F01");
	params["number of variables"] = 2;
	params["algorithm name"] = std::string("Canonical-DE");
	params["population size"] = int(20);
	params["maximum evaluations"] = int(1000);
	int id_param1 = ADD_PARAM(params);
	int id_pro1 = ADD_PRO(id_param1, 0.1);
	GET_PRO(id_pro1).initialize();
	int id_alg1 = ADD_ALG(id_param1, 0.1, 0.01, -1);
	GET_ALG(id_alg1).initialize();



	ParamMap params2;
	params2["problem name"] = std::string("BBOB_F02");
	params2["number of variables"] = 2;
	params2["algorithm name"] = std::string("Canonical-DE");
	params2["population size"] = 20;
	params2["maximum evaluations"] = int(1000);
	int id_param2 = ADD_PARAM(params);
	int id_pro2 = ADD_PRO(id_param2, 0.1);
	GET_PRO(id_pro2).initialize();
	int id_alg2 = ADD_ALG(id_param2, 0.1, 0.01, -1);
	GET_ALG(id_alg2).initialize();

	std::vector<std::thread> thrds;
	thrds.push_back(std::thread(runAlgExample, id_alg1));
	thrds.push_back(std::thread(runAlgExample, id_alg2));
	for (auto& it : thrds) it.join();

	DEL_ALG(id_alg1);
	DEL_PRO(id_pro1);
	DEL_PARAM(id_param1);
	DEL_ALG(id_alg2);
	DEL_PRO(id_pro2);
	DEL_PARAM(id_param2);

}





TEST_CASE("example3", "[case3]")
{
	InstanceManager::ms_instance_manager.reset(new InstanceManager);
	ParamMap params;
	params["problem name"] = std::string("BBOB_F01");
	params["number of variables"] = 2;
	params["algorithm name"] = std::string("Canonical-DE");
	params["population size"] = int(20);
	params["maximum evaluations"] = int(1000);
	int num_runs(20);
	params["number of runs"] = num_runs;

	int id_param = ADD_PARAM(params);
	int id_rcr = ADD_RCR(id_param);
	std::list<int> ids_algs, ids_pros;
	std::vector<std::thread> thrds;

	for (size_t i = 0; i < num_runs; i++) {
		int id_pro = ADD_PRO(id_param, Real(i + 1) / (num_runs + 1));
		int id_alg = ADD_ALG(id_param, id_pro, Real(i + 1) / (num_runs + 1), id_rcr);
		GET_PRO(id_pro).initialize();
		GET_ALG(id_alg).initialize();
		ids_pros.push_back(id_pro);
		ids_algs.push_back(id_alg);
		thrds.emplace_back(runAlgExample, id_alg);
	}
	for (auto& t : thrds)
		t.join();

	GET_RCR(id_rcr).outputData(ids_pros, ids_algs);
	/* release memory  */
	for (int id_alg : ids_algs) DEL_ALG(id_alg);
	for (int id_pro : ids_pros) DEL_PRO(id_pro);

}






