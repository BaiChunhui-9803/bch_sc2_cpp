#include "global.h"

namespace ofec {
	Factory<Problem> g_reg_problem;
	Factory<Algorithm> g_reg_algorithm;

	std::map<std::string, std::set<std::string>> g_alg_for_pro;
	std::set<std::string> g_param_omit;
	std::map<std::string, std::string> g_param_abbr;
	std::string g_working_dir = OFEC_DIR;
	std::mutex g_cout_mutex;
}