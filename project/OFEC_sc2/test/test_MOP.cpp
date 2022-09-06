#include <iomanip>
#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include "../core/algorithm/population.h"
#include "../core/algorithm/individual.h"

using namespace ofec;

TEST_CASE("MOP", "[GLT1]")
{
	InstanceManager::ms_instance_manager.reset(new InstanceManager);


	std::vector<std::string> alg_names = { "MOEA/D-DE","MOEA/D-SBX","CDGMOEA","GrEA","NSGAII-DE","NSGAII-SBX","NSGAIII-SBX" };
	InstanceManager::ms_instance_manager.reset(new InstanceManager);

	std::vector<int> DTLZ_objs = { 3,5,8,10,15 };

	std::vector<int> objs_idxs = { 2,3,8};
	

	for (const auto& alg : alg_names) {

		


		{
			ParamMap params;
			params["algorithm name"] = alg;
			params["population size"] = int(200);
			params["maximum evaluations"] = int(5250);
			params["number of variables"] = 7;
            params["kFactor"] = 2;
		    params["lFactor"] = 2;
			std::string pro_name = "DTLZ";
			std::vector<int> dtlz_id = { 1,2,3,4,7 };
			
			for (auto& pro_id : dtlz_id) {
				std::string cur_pro_name = pro_name + std::to_string(pro_id);
				params["problem name"] = cur_pro_name;
				for (auto& obj_idx : DTLZ_objs) {
					params["number of objectives"] = obj_idx;
					if (alg.substr(0, 4) == "MOEA") {
						if (obj_idx >= 5) continue;
						params["population size"] = int(105);
		             }
					else if (alg == "CDGMOEA") {
						if (obj_idx > 3)continue;
					}
					else {
						params["population size"] = int(200);
					}



					std::cout << "alg name\t" << alg << "\t" << "problem_names\t" << cur_pro_name << "\t" << "obj number\t" << obj_idx << std::endl;


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


		}




		{
			ParamMap params;
			params["algorithm name"] = alg;
			params["population size"] = int(200);
			params["maximum evaluations"] = int(5250);
			params["number of variables"] = 7;
			params["kFactor"] = 2;
			params["lFactor"] = 2;
			std::string pro_name = "MOP_GLT";

			for (int pro_id = 1; pro_id <= 6; ++pro_id) {
				std::string cur_pro_name = pro_name + std::to_string(pro_id);
				params["problem name"] = cur_pro_name;
				for (auto& obj_idx : objs_idxs) {
					if ((pro_id == 5 ||pro_id==6)&& obj_idx <= 2)continue;
					params["number of objectives"] = obj_idx;
					if (alg.substr(0, 4) == "MOEA") {
						if (obj_idx !=3) continue;
						if(obj_idx==3) params["population size"] = int(105);
						//else if(obj_idx==2) params["population size"] = int(20);
					}
					else if (alg == "CDGMOEA") {
						if (obj_idx > 3) continue;
					}
					else {
						params["population size"] = int(200);
					}


					std::cout << "alg name\t" << alg << "\t" << "problem_names\t" << cur_pro_name << "\t" << "obj number\t" << obj_idx << std::endl;




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



		}

	}


	/*ParamMap params;
	params["problem name"] = std::string("MOP_GLT1");
	params["number of variables"] = 6;
	params["number of objectives"] = 2;
	params["algorithm name"] = std::string("CDGMOEA");
	params["population size"] = int(20);
	params["maximum evaluations"] = int(2000);
	int id_param = ADD_PARAM(params);
	int id_pro = ADD_PRO(id_param, 0.1);
	GET_PRO(id_pro).initialize();
	int id_alg = ADD_ALG(id_param, 0.1, 0.01, -1);
	GET_ALG(id_alg).initialize();
	GET_ALG(id_alg).run();

	DEL_ALG(id_alg);
	DEL_PRO(id_pro);
	DEL_PARAM(id_param);*/

	//for (size_t i = 5; i <6; i++) {
	//	ParamMap params;
	//	if (i == 5) {
	//		std::string inx = std::to_string(i+2);
	//		params["problem name"] = std::string("DTLZ") + inx;
	//	}
	//	else {
	//		std::string inx = std::to_string(i);
	//		params["problem name"] = std::string("DTLZ") + inx;
	//	}
	//	/*std::string inx = std::to_string(i);
	//	params["problem name"] = std::string("WFG") + inx;*/
	//	if (i == 5) {
	//		params["number of variables"] = 7;
	//		params["number of objectives"] = 3;
	//		/*params["kFactor"] = 2;
	//		params["lFactor"] = 2;*/
	//		params["population size"] = int(200);
	//		params["maximum evaluations"] = int(5250);
	//	}
	//	else {
	//		params["number of variables"] = 6;
	//		params["number of objectives"] = 2;
	//		params["population size"] = int(200);
	//		params["maximum evaluations"] = int(2000);
	//	}
	//	params["algorithm name"] = std::string("NSGAIII-SBX");
	//	int id_param = ADD_PARAM(params);
	//	int id_pro = ADD_PRO(id_param, 0.1);
	//	GET_PRO(id_pro).initialize();
	//	int id_alg = ADD_ALG(id_param, 0.1, 0.01, -1);
	//	GET_ALG(id_alg).initialize();
	//	GET_ALG(id_alg).run();

	//	DEL_ALG(id_alg);
	//	DEL_PRO(id_pro);
	//	DEL_PARAM(id_param);
	//}

}


//TEST_CASE("MOP", "[ZDT1]")
//{
//    /*global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));
//    size_t size_var, num_obj;
//
//    size_var = 8, num_obj = 3;
//    global::ms_global->m_problem.reset(new WFG1("MOP_WFG1", size_var, num_obj));
//    global::ms_global->m_problem->initialize();
//    dynamic_cast<WFG1*>(global::ms_global->m_problem.get())->set_k(4);
//
//    solution<variable_vector<real>,real> s(num_obj, 0, size_var);
//	for (int i = 0; i < size_var; ++i) {
//		s.variable()[i] = 2 * (i + 1);
//	}
//	s.evaluate(false, caller::Problem);
//
//	for (int i = 0; i < s.objective_size(); ++i)
//		std::cout << std::setprecision(6) << s.objective(i) << " ";
//	std::cout << std::endl;
//
//	population<individual<>> pop(100, size_var);
//	pop.initialize();
//	pop.evaluate();
//	pop.sort();*/
//}

//TEST_CASE("MOP", "[WFG]")
//{
//    global::ms_global = std::unique_ptr<global>(new global(1, 0.5, 0.5));
//    size_t size_var, num_obj;
//
//    size_var = 8, num_obj = 3;
//    global::ms_global->m_problem.reset(new WFG1("MOP_WFG1", size_var, num_obj));
//    global::ms_global->m_problem->initialize();
//    dynamic_cast<WFG1*>(global::ms_global->m_problem.get())->set_k(4);
//
//    solution<variable_vector<real>,real> s(num_obj, 0, size_var);
//	for (int i = 0; i < size_var; ++i) {
//		s.variable()[i] = 2 * (i + 1);
//	}
//	s.evaluate(false, caller::Problem);
//
//	for (int i = 0; i < s.objective_size(); ++i)
//		std::cout << std::setprecision(6) << s.objective(i) << " ";
//	std::cout << std::endl;
//
//	population<individual<>> pop(100, size_var);
//	pop.initialize();
//	pop.evaluate();
//	pop.sort();
//}
