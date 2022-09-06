#include "F7_shifted_rotated_griewank_no_bound.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedRotatedGriewankNoBound::initialize_() {
			Griewank::initialize_();
			setDomain(-1000, 1000);
			for (size_t i = 0; i < m_num_vars; i++)
				m_domain[i].limited = false;
			setInitialDomain(0, 600.);
			setBias(-180);
			setConditionNumber(3);
			setOriginalGlobalOpt();
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			loadRotation("/instance/problem/continuous/global/cec2005/data/");
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-2;
			m_variable_niche_radius = 1e-4 * 1000 * m_num_vars;
		}
	}
}