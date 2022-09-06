#include "f3_sr_ackley.h"

namespace ofec {
	namespace CEC2015 {
		void F3_SR_ackley::initialize_() {
			Ackley::initialize_();
			setConditionNumber(1.);
			setBias(300.);
			loadTranslation("/instance/problem/continuous/global/cec2015/data/");
			loadRotation("/instance/problem/continuous/global/cec2015/data/");
			setGlobalOpt(m_translation.data());
		}
	}
}