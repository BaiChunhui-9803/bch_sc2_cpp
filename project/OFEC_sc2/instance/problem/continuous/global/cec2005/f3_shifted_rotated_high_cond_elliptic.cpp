#include "F3_shifted_rotated_high_cond_elliptic.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedRotatedHighCondElliptic::initialize_() {
			Elliptic::initialize_();
			setBias(-450);
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			loadRotation("/instance/problem/continuous/global/cec2005/data/");	
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-6;
		}
	}
}