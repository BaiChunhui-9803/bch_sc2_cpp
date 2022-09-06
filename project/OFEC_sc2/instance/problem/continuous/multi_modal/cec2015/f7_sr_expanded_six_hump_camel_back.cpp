#include "F7_SR_expanded_six_hump_camel_back.h"

namespace ofec {
	namespace CEC2015 {
		F7_SR_expanded_six_hump_camel_back::F7_SR_expanded_six_hump_camel_back(const ParamMap &v) :
			F7_SR_expanded_six_hump_camel_back((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		F7_SR_expanded_six_hump_camel_back::F7_SR_expanded_six_hump_camel_back(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			
		}

		void F7_SR_expanded_six_hump_camel_back::initialize() {
			setDomain(-100, 100);
			setInitialDomain(-100, 100);
			
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			setConditionNumber(1.0);
			m_variable_monitor = true;
			setBias(700.);
			setScale(20.);
			load_optima("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadTranslation("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadRotation("/instance/problem/continuous/multi_modal/CEC2015/data/");

			// 2^(Dim/2) gopt

			evaluate_optima();
			m_initialized = true;
		}
		int F7_SR_expanded_six_hump_camel_back::evaluateObjective(Real *x, std::vector<Real> &obj) {

			size_t i;

			for (i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];
			scale(x);
			rotate(x);

			obj[0] = 0.0;

			for (i = 0; i<m_num_vars - 1; i = i + 2)
			{
				x[i] += 0.089842;
				x[i + 1] += -0.712656;//shift to orgin
				obj[0] += ((4.0 - 2.1*pow(x[i], 2.0) + pow(x[i], 4.0) / 3.0)*pow(x[i], 2.0) + x[i] * x[i + 1] + ((-4.0 + 4.0*pow(x[i + 1], 2.0))*pow(x[i + 1], 2.0)))*4.0;
			}
			obj[0] += 4.126514*m_num_vars / 2.0;
			obj[0] += m_bias;
			return kNormalEval;
		}
	}
}