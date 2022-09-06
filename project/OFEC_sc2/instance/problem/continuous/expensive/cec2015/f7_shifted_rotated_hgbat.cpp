#include "F7_shifted_rotated_HGBat.h"

namespace ofec {
	namespace CEC2015 {
		F7_shifted_rotated_HGBat::F7_shifted_rotated_HGBat(const ParamMap &v) :
			F7_shifted_rotated_HGBat((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		F7_shifted_rotated_HGBat::F7_shifted_rotated_HGBat(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			HGBat(name, size_var, size_obj) {

			
		}

		void F7_shifted_rotated_HGBat::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			setOriginalGlobalOpt();
			setBias(700);

			
			loadTranslation("/instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			loadRotation("/instance/problem/continuous/expensive/CEC2015/data/");
			
			setGlobalOpt(m_translation.data());
			m_initialized = true;
		}
		int F7_shifted_rotated_HGBat::evaluateObjective(Real *x, std::vector<Real> &obj) {
			return HGBat::evaluateObjective(x, obj);
		}
	}
}