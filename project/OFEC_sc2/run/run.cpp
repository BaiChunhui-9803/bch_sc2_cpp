#include "custom_method.h"
#include "prime_method.h"
#include "../core/global.h"
#include "../core/instance_manager.h"
#include <fstream>
#include <list>
#include <iostream>
#include <chrono>
#include <thread>

namespace ofec {
	void run() {
		InstanceManager::ms_instance_manager.reset(new InstanceManager);
		registerParamAbbr();
		customizeFileName();
		ParamMap params;
		std::string argvs;
		std::ifstream task_file(g_working_dir + "/tasks.txt");
		while (std::getline(task_file, argvs)) {
			if (abbrToParameter(argvs, params) &&
				checkValidation(std::get<std::string>(params.at("algorithm name")), std::get<std::string>(params.at("problem name"))))
			{
				std::cout << "[Task start] Parameters: " << argvs << std::endl;
				std::vector<std::thread> thrds;
				size_t num_runs = std::get<int>(params.at("number of runs"));
				int id_param = ADD_PARAM(params);
				int id_rcr = ADD_RCR(id_param);
				std::list<int> ids_algs, ids_pros;
				auto start = std::chrono::steady_clock::now();
				for (size_t i = 0; i < num_runs; i++) {
					Real seed = Real(i + 1) / (num_runs + 1);
					int id_pro = ADD_PRO(id_param, seed);
					int id_alg = ADD_ALG(id_param, id_pro, seed, id_rcr);
					GET_PRO(id_pro).initialize();
					GET_ALG(id_alg).initialize();
					ids_pros.push_back(id_pro);
					ids_algs.push_back(id_alg);
				}
				for (int id_alg : ids_algs)
					thrds.emplace_back(runAlg, id_alg);
				for (auto& t : thrds)
					t.join();
				std::cout << "############ Output data" << std::endl;
				GET_RCR(id_rcr).outputData(ids_pros, ids_algs);
				/* release memory  */
				for (int id_alg : ids_algs) DEL_ALG(id_alg);
				for (int id_pro : ids_pros) DEL_PRO(id_pro);
				DEL_RCR(id_rcr);
				DEL_PARAM(id_param);
				auto end = std::chrono::steady_clock::now();
				float diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				std::cout << "[Task finished] Time used: " << diff / 1000 << " s" << std::endl;
			}
		}
	}
}