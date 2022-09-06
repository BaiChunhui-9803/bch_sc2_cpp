#include "f4_sr_rastrigin.h"

namespace ofec {
	namespace CEC2015 {
		void F4_SR_rastrigin::initialize_() {
			Rastrigin::initialize_();
			setConditionNumber(1.);
			setBias(400.);
			loadTranslation("/instance/problem/continuous/global/cec2015/data/");
			loadRotation("/instance/problem/continuous/global/cec2015/data/");
			setScale(100. / 5.12);
			setGlobalOpt(m_translation.data());
		}
	}
}