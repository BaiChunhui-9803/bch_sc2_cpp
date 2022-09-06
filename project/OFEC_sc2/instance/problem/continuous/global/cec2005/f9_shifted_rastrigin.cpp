#include "F9_shifted_rastrigin.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedRastrigin::initialize_() {
			Rastrigin::initialize_();
			setDomain(-5, 5);
			setBias(-330);
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-2;
			m_variable_niche_radius = 1e-4 * 5 * m_num_vars;
		}
	}
}