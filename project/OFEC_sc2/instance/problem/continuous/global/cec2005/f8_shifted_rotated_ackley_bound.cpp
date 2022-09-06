#include "F8_shifted_rotated_ackley_bound.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedRotatedAckleyBound::initialize_() {
			Ackley::initialize_();
			setDomain(-32, 32);
			setOriginalGlobalOpt();
			setBias(-140);
			setConditionNumber(100);	
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			for (size_t j = 1; j < m_num_vars / 2 + 1; ++j) {
				m_translation[(2 * j - 1) - 1] = -32;
				m_translation[2 * j - 1] = GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-32, 32);
			}
			loadRotation("/instance/problem/continuous/global/cec2005/data/");	
			setGlobalOpt(m_translation.data());
			m_objective_accuracy = 1.0e-2;
			m_variable_niche_radius = 1e-4 * 32 * m_num_vars;
		}
	}
}