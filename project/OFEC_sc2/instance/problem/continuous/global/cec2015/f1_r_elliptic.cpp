#include "f1_r_elliptic.h"

namespace ofec {
	namespace CEC2015 {
		void F1_R_elliptic::initialize_() {
			Elliptic::initialize_();
			setConditionNumber(1.);
			setBias(100.);
			loadTranslation("/instance/problem/continuous/global/cec2015/data/");
			loadRotation("/instance/problem/continuous/global/cec2015/data/");
			setGlobalOpt(m_translation.data());
		}
	}
}