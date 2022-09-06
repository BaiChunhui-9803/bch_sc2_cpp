#include "DMOPs.h"

namespace ofec {
	DMOPs::DMOPs(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, 2), continuous(name, size_var, 2) {

	}

	Real DMOPs::get_time() {
		return ((size_t)m_effective_eval / (size_t)global::ms_arg.find("changeFre")->second) / (Real)m_severity;
	}

	bool DMOPs::time_changed() {
		if ((size_t)m_effective_eval % (size_t)global::ms_arg.find("changeFre")->second == 0)
			return true;
		/*if (m_effective_eval>0 && ((int)m_effective_eval) % (m_duration_gen * (int)global::ms_arg.find("population size")->second) == 0 )
		return true;*/
		else
			return false;
	}


}