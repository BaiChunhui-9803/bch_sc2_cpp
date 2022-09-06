#include "F11_shifted_rotated_weierstrass.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedRotatedWeierstrass::initialize_() {
			Weierstrass::initialize_();
			setConditionNumber(5);
			setBias(90);
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			loadRotation("/instance/problem/continuous/global/cec2005/data/");
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-2;
		}
	}
}