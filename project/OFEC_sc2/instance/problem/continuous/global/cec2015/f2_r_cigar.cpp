#include "f2_r_cigar.h"

namespace ofec {
	namespace CEC2015 {
		void F2_R_cigar::initialize_() {
			BentCigar::initialize_();
			setConditionNumber(1.);
			setBias(200.);
			loadTranslation("/instance/problem/continuous/global/cec2015/data/");
			loadRotation("/instance/problem/continuous/global/cec2015/data/");	
			setGlobalOpt(m_translation.data());
		}
	}
}