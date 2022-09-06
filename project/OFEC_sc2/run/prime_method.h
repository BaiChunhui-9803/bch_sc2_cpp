#ifndef OFEC_SYSTEM_PREF_H
#define OFEC_SYSTEM_PREF_H

#include <string>
#include <vector>
#include "../utility/typevar/typevar.h"

namespace ofec {
	void setAlgForPro();
	bool abbrToParameter(const std::string& argvs, ParamMap& params);
	bool checkValidation(const std::string& alg_name, const std::string& pro_name);
	void runAlg(int id_alg);
}

#endif // !OFEC_SYSTEM_PREF_H
