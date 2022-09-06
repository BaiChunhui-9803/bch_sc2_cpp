#include "F8_SR_modified_vincent.h"

namespace ofec {
	namespace CEC2015 {
		F8_SR_modified_vincent::F8_SR_modified_vincent(const ParamMap &v) :
			F8_SR_modified_vincent((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		F8_SR_modified_vincent::F8_SR_modified_vincent(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
		
		}

		void F8_SR_modified_vincent::initialize() {
			setDomain(-100, 100);
			setInitialDomain(-100, 100);
			
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			setConditionNumber(1.0);
			m_variable_monitor = true;
			setBias(800.);
			setScale(5.);
			load_optima("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadTranslation("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadRotation("/instance/problem/continuous/multi_modal/CEC2015/data/");

			// 6^Dim gopt

			evaluate_optima();
			m_initialized = true;
		}
		int F8_SR_modified_vincent::evaluateObjective(Real *x, std::vector<Real> &obj) {

			size_t i;

			for (i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];
			scale(x);
			rotate(x);

			obj[0] = 0.0;

			for (i = 0; i<m_num_vars; i++)
			{
				x[i] += 4.1112;//shift to orgin
				if ((x[i] >= 0.25)&(x[i] <= 10.0))
				{
					obj[0] += -sin(10.0*log(x[i]));
				}
				else if (x[i]<0.25)
				{
					obj[0] += pow(0.25 - x[i], 2.0) - sin(10.0*log(2.5));
				}
				else
				{
					obj[0] += pow(x[i] - 10, 2.0) - sin(10.0*log(10.0));
				}
			}
			obj[0] /= m_num_vars;
			obj[0] += 1 + m_bias;
			return kNormalEval;
		}
	}
}