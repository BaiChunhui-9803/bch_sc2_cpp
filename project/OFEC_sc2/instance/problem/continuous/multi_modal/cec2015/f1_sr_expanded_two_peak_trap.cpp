#include "F1_SR_expanded_two_peak_trap.h"

namespace ofec {
	namespace CEC2015 {
		F1_SR_expanded_two_peak_trap::F1_SR_expanded_two_peak_trap(const ParamMap &v) :
			F1_SR_expanded_two_peak_trap((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		F1_SR_expanded_two_peak_trap::F1_SR_expanded_two_peak_trap(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			
		}

		void F1_SR_expanded_two_peak_trap::initialize() {
			setDomain(-100, 100);
			setInitialDomain(-100, 100);
			
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			setConditionNumber(1.0);
			m_variable_monitor = true;
			setBias(100.);

			load_optima("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadTranslation("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadRotation("/instance/problem/continuous/multi_modal/CEC2015/data/");

			// 5  Dim : 1 gopt and 15 lopt
			// 10 Dim : 1 gopt and 55 lopt
			// 20 Dim : 1 gopt and 210 lopt
			evaluate_optima();
			m_initialized = true;
			
		}
		int F1_SR_expanded_two_peak_trap::evaluateObjective(Real *x, std::vector<Real> &obj) {
			
			size_t i;

			for (i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];
			rotate(x);

			obj[0] = 0.0;

			for (i = 0; i<m_num_vars; ++i)
			{
				x[i] += 20.0;
				if ((x[i]<15.0)&(x[i] >= 0.0))
				{
					obj[0] += -(160.0 / 15.0)*(15.0 - x[i]);
				}
				else if ((x[i] <= 20.0)&(x[i] >= 15.0))
				{
					obj[0] += -40.0*(x[i] - 15.0);
				}
				else if (x[i]<0.0)
				{
					obj[0] += -160.0 + pow(x[i], 2.0);
				}
				else
				{
					obj[0] += -200.0 + pow(x[i] - 20.0, 2.0);
				}
			}
			obj[0] += 200.0*m_num_vars;
			obj[0] += m_bias;
			return kNormalEval;
		}
	}
}