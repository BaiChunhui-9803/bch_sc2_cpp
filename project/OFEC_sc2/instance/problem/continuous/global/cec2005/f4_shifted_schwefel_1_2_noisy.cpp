#include "F4_shifted_schwefel_1_2_noisy.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedSchwefel_1_2_Noisy::initialize_() {
			Schwefel_1_2::initialize_();
			setOriginalGlobalOpt();
			setBias(-450);	
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path		
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-6;
		}

		void ShiftedSchwefel_1_2_Noisy::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
			Schwefel_1_2::evaluateOriginalObj(x, obj);
			obj[0] = (obj[0] - m_bias)*fabs(GET_RND(m_id_rnd).normal.next()) + m_bias;
		}
	}
}