#include "F5_shifted_rotated_katsuura.h"

namespace ofec {
	namespace CEC2015 {
		F5_shifted_rotated_katsuura::F5_shifted_rotated_katsuura(const ParamMap &v) :
			F5_shifted_rotated_katsuura((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		F5_shifted_rotated_katsuura::F5_shifted_rotated_katsuura(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			katsuura(name, size_var, size_obj) {

			
		}

		void F5_shifted_rotated_katsuura::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			setOriginalGlobalOpt();

			
			setBias(500);

			loadTranslation("/instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			loadRotation("/instance/problem/continuous/expensive/CEC2015/data/");
			
			setGlobalOpt(m_translation.data());
			m_initialized = true;
		}
		int F5_shifted_rotated_katsuura::evaluateObjective(Real *x, std::vector<Real> &obj) {
			return katsuura::evaluateObjective(x, obj);
		}
	}
}