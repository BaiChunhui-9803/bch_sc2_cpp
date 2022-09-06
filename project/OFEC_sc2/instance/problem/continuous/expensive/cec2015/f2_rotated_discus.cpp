#include "F2_rotated_discus.h"

namespace ofec {
	namespace CEC2015 {
		F2_rotated_discus::F2_rotated_discus(const ParamMap &v) :F2_rotated_discus((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		F2_rotated_discus::F2_rotated_discus(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			discus(name, size_var, size_obj) {

			
		}

		void F2_rotated_discus::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);

			setBias(200);
			setOriginalGlobalOpt();
			loadRotation("/instance/problem/continuous/expensive/CEC2015/data/");
			
			setGlobalOpt();
			m_initialized = true;
		}
		int F2_rotated_discus::evaluateObjective(Real *x, std::vector<Real> &obj) {
			return discus::evaluateObjective(x, obj);
		}
	}
}