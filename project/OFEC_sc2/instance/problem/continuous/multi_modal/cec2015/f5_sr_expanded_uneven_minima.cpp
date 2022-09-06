#include "F5_SR_expanded_uneven_minima.h"

namespace ofec {
	namespace CEC2015 {
		F5_SR_expanded_uneven_minima::F5_SR_expanded_uneven_minima(const ParamMap &v) :
			F5_SR_expanded_uneven_minima((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		F5_SR_expanded_uneven_minima::F5_SR_expanded_uneven_minima(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			
		}

		void F5_SR_expanded_uneven_minima::initialize() {
			setDomain(-100, 100);
			setInitialDomain(-100, 100);
			
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			setConditionNumber(1.0);
			m_variable_monitor = true;
			setBias(500.);
			setScale(20.);
			load_optima("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadTranslation("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadRotation("/instance/problem/continuous/multi_modal/CEC2015/data/");

			// 5^Dim gopt
			 
			evaluate_optima();
			m_initialized = true;
		}
		int F5_SR_expanded_uneven_minima::evaluateObjective(Real *x, std::vector<Real> &obj) {

			size_t i;

			for (i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];
			scale(x);
			rotate(x);

			obj[0] = 0.0;

			for (i = 0; i<m_num_vars; i++)
			{
				x[i] += 0.079699392688696;//pow(0.15,4.0/3.0);//shift to orgin
				if ((x[i] <= 1.0)&(x[i] >= 0.0))
				{
					obj[0] -= pow(sin(5.0*OFEC_PI*(pow(x[i], 0.75) - 0.05)), 6.0);
				}
				else
				{
					obj[0] += pow(x[i], 2.0);
				}
			}

			obj[0] += 1.0*m_num_vars;
			obj[0] += m_bias;
			return kNormalEval;
		}
	}
}