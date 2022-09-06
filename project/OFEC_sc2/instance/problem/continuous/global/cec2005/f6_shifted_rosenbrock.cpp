#include "F6_shifted_rosenbrock.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedRosenbrock::initialize_() {
			Rosenbrock::initialize_();
			setDomain(-100, 100);
			std::vector<Real> v(m_num_vars, 1);
			setOriginalGlobalOpt(v.data());
			setBias(390);
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-2;
			m_variable_niche_radius = 1e-4 * 100 * m_num_vars;
		}
	}
}