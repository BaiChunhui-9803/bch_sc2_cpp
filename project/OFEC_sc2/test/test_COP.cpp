#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include "../core/algorithm/population.h"
#include "../core/algorithm/individual.h"

#include<string>
#include<iostream>
using namespace ofec;



TEST_CASE("constrained optimization", "[CEC2017][DCNSGAII]")
{
	InstanceManager::ms_instance_manager.reset(new InstanceManager);


	std::string pro_name = "COP_CEC2017_F";
	for (int pro_id(1); pro_id <= 28; ++pro_id) {
		char id1 = '0' + pro_id / 10;
		char id2 = '0' + pro_id % 10;
		std::string cur_pro_name = pro_name+id1+id2;
		
		std::cout << "cur problem names\t" << cur_pro_name << std::endl;

		ParamMap params;
		params["problem name"] = cur_pro_name;
		params["number of variables"] = 3;
		params["number of objectives"] = 2;

		params["algorithm name"] = std::string("DCNSGAII-DE");
		params["population size"] = int(40);
		params["maximum evaluations"] = int(5000);
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

}