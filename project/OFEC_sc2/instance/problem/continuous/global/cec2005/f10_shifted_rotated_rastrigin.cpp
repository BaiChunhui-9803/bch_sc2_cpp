#include "F10_shifted_rotated_rastrigin.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedRotatedRastrigin::initialize_() {
			Rastrigin::initialize_();
			setDomain(-5, 5);
			setBias(-330);
			setConditionNumber(2);
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			loadRotation("/instance/problem/continuous/global/cec2005/data/");	
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-2;
		}
	}
}