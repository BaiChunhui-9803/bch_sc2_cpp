#include "F11_global_composition3.h"
#include "../../global/classical/HGBat.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/schwefel.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/elliptic.h"

namespace ofec {
	namespace CEC2015 {
		F11_global_composition3::F11_global_composition3(const ParamMap &v) :
			F11_global_composition3((v.at("problem name")), (v.at("number of variables")), 1) {
			
			
		}
		F11_global_composition3::F11_global_composition3(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {
			
			
		}

		void F11_global_composition3::setFunction() {
			basic_func f(5);
			f[0] = &create_function<HGBat>;
			f[1] = &create_function<Rastrigin>;
			f[2] = &create_function<Schwefel>;
			f[3] = &create_function<Weierstrass>;
			f[4] = &create_function<Elliptic>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<Function*>(f[i](""));
				m_function[i]->initialize();
				m_function[i]->setBias(0);
			}

			for (auto &i : m_function)
				i->setConditionNumber(1.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 10;
			m_converge_severity[2] = 10;
			m_converge_severity[3] = 20;
			m_converge_severity[4] = 20;

			m_height[0] = 10;
			m_height[1] = 10;
			m_height[2] = 2.5;
			m_height[3] = 25;
			m_height[4] = 1e-6;

			Real temp = 0;
			for (auto &i : m_f_bias)
				i = temp;
			temp += 100;

		}

		void F11_global_composition3::initialize() {
			m_num_function = 5;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			m_variable_monitor = true;
			setFunction();
			
			loadTranslation("/instance/problem/continuous/global/CEC2015/data/");
			loadRotation("/instance/problem/continuous/global/CEC2015/data/");
			for (auto &i : m_function) {
				i->get_optima().clear();
				i->setGlobalOpt(i->translation().data());
			}
			// Set optimal solution
            Solution<VarVec<Real>, Real> s(m_num_objs, num_constraints(), m_num_vars);
            s.variable() = m_function[0]->get_optima().variable(0);

            m_optima.append(s.variable());
            m_optima.setVariableGiven(true);

            s.evaluate(false, caller::Problem);
            m_optima.append(s.objective());

			add_tag(problem_tag::MMOP);
			m_initialized = true;
		}

		int F11_global_composition3::evaluateObjective(Real *x, std::vector<Real> &obj) {
			std::vector<Real> x_(m_num_vars);
			std::copy(x, x + m_num_vars, x_.begin());
			std::vector<Real> weight(m_num_function, 0);

			set_weight(weight, x_);
			std::vector<Real> fit(m_num_function);
			VarVec<Real> temp_var(m_num_vars);
			std::vector<Real> temp_obj(m_num_objs);
            Solution<VarVec<Real>, Real> s(m_num_objs, num_constraints(), m_num_vars);
            for (size_t i = 0; i < m_num_function; ++i) { // calculate objective value for each function
				s.variable().vect() = x_;
				
				m_function[i]->evaluate_(s, caller::Problem, false, false);
				fit[i] = s.objective()[0];

			}
			Real sumw = 0;
			for (size_t i = 0; i < m_num_function; ++i)
				sumw += weight[i];
			for (size_t i = 0; i < m_num_function; ++i)
				weight[i] /= sumw;

			Real temp = 0;
			for (size_t i = 0; i < m_num_function; ++i) {
				temp += weight[i] * (m_height[i] * fit[i] + m_f_bias[i]);
			}

			obj[0] = temp;
			obj[0] += 1100;
			return kNormalEval;
		}

	}
}