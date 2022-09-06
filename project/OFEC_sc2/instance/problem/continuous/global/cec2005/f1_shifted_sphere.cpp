#include "F1_shifted_sphere.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedSphere::initialize_() {
			Sphere::initialize_();
			setBias(-450);
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path	
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-6;
		}
	}
}