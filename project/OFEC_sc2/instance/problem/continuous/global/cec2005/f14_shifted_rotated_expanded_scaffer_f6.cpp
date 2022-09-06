#include "F14_shifted_rotated_expanded_scaffer_F6.h"
#include <numeric>

namespace ofec {
	namespace cec2005 {
		void ShiftedRotatedExpandedScafferF6::initialize_() {
			ScafferF6::initialize_();
			setOriginalGlobalOpt();				
			setConditionNumber(3);
			setBias(-300);			
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			loadRotation("/instance/problem/continuous/global/cec2005/data/");		
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-2;
		}
	}
}