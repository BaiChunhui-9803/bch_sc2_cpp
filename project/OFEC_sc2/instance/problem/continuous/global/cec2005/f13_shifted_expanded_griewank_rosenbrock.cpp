#include "F13_shifted_expanded_griewank_rosenbrock.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	namespace cec2005 {
		void ShiftedExpandedGriewankRosenbrock::initialize_() {
			Function::initialize_();
			resizeVariable(std::get<int>(GET_PARAM(m_id_param).at("number of variables")));
			setDomain(-3, 1);
			setOriginalGlobalOpt();
			setBias(-130);
			loadTranslation("/instance/problem/continuous/global/cec2005/data/");  //data path
			setGlobalOpt(m_translation.data());
			m_variable_niche_radius = 1e-4 * 2 * m_num_vars;
			m_objective_accuracy = 1.0e-2;
		}

		void ShiftedExpandedGriewankRosenbrock::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
			Real result = 0;
			for (size_t i = 0; i < m_num_vars; ++i) {
				Real result_f2 = 0;
				Real result_f8 = 0;
				Real x_front = x[i] + 1;
				Real x_back = x[(i + 1) % m_num_vars] + 1;
				result_f2 += 100 * pow((x_back - x_front * x_front), 2.0) + (x_front - 1) * (x_front - 1);
				result_f8 += result_f2 * result_f2 / 4000.0 - cos(result_f2 / sqrt((Real)(i + 1))) + 1;
				result += result_f8;
			}
			result += m_bias;
			obj[0] = result;
		}
	}
}