#include "f5_sr_schwefel.h"

namespace ofec {
	namespace CEC2015 {
		void F5_SR_schwefel::initialize_() {
			Schwefel::initialize_();
			setConditionNumber(1.);
			setBias(500.);
			loadTranslation("/instance/problem/continuous/global/cec2015/data/");
			loadRotation("/instance/problem/continuous/global/cec2015/data/");
			setScale(1. / 10.);
			setGlobalOpt(m_translation.data());
		}
	}
}