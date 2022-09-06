#include "prime_method.h"
#include "../core/global.h"
#include "../core/instance_manager.h"
#include "../core/algorithm/algorithm.h"
#include <sstream>
#include <iostream>
#include <iomanip>

namespace ofec {
	bool abbrToParameter(const std::string& argvs, ParamMap& params) {
		params.clear();
		std::string letter;
		for (auto i = 'a'; i <= 'z'; i++) {
			letter += i;
			letter += static_cast<char>(i - 32);
		}
		std::string remove = "\r\t\n\b\v";
		letter += "\\/:";
		std::stringstream ss_argvs(argvs);
		std::string argv;
		while (std::getline(ss_argvs, argv, ' ')) {
			while (size_t pos = argv.find_first_of(remove)) {
				if (std::string::npos == pos) break;
				argv.erase(argv.begin() + pos);
			}
			size_t pos = argv.find('=');
			if (pos == std::string::npos)  throw MyExcept("Invalid argument:gSetGlobalParameters()");
			std::string value = argv.substr(pos + 1, argv.size() - 1), name = argv.substr(0, pos);
			if (g_param_abbr.count(name) == 0)
				throw MyExcept("Invalid argument @setGlobalParameters()");
			if (value == "true") {
				params[g_param_abbr.at(name)] = true;
			}
			else if (value == "false") {
				params[g_param_abbr.at(name)] = false;
			}
			else if (value.find_first_of(letter) != std::string::npos) {
				if (value.size() == 1)
					params[g_param_abbr.at(name)] = value[0];
				else
					params[g_param_abbr.at(name)] = value;
			}
			else if (std::string::npos != argv.find('.', pos + 1)) {
				Real val = atof(value.c_str());
				params[g_param_abbr.at(name)] = val;
			}
			else {
				int val = atoi(value.c_str());
				params[g_param_abbr.at(name)] = val;
			}
		}
		return true;
	}

	void setAlgForPro() {
		for (auto &alg : g_reg_algorithm.get()) {
			for (auto &pro : g_reg_problem.get()) {
				for (auto it = pro.second.second.begin();;) {
					if (!alg.second.second.count(*it))
						break;
					if (++it == pro.second.second.end()) {
						g_alg_for_pro[alg.first].insert(pro.first);
						break;
					}
				}
			}
		}
	}

	bool checkValidation(const std::string& alg_name, const std::string& pro_name) {
		if (g_alg_for_pro.count(alg_name) > 0 && g_alg_for_pro.at(alg_name).count(pro_name) > 0)
			return true;
		else
			return false;
	}
	
	void runAlg(int id_alg) {
		{
			std::unique_lock<std::mutex> lock(g_cout_mutex);
			std::cout << "Alg ID " << std::right << std::setw(2) << id_alg << " started." << std::endl;
		}
		GET_ALG(id_alg).run();
		{
			std::unique_lock<std::mutex> lock(g_cout_mutex);
			std::cout << "Alg ID " 
				<< std::right << std::setw(2) << id_alg << " Terminated. "
				<< std::right << std::setw(7) << GET_ALG(id_alg).numEffectiveEvals() << " FEs Costed." << std::endl;
		}
	}
}