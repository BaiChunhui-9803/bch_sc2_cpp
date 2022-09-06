#include "F2_shifted_schwefel_1_2.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedSchwefel_1_2::initialize_() {
			Schwefel_1_2::initialize_();
			setBias(-450);
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-6;
		}
	}
}