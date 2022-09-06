#include "F15_global_composition7.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/happy_cat.h"
#include "../../global/classical/schwefel.h"
#include "../../global/classical/rosenbrock.h"
#include "../../global/classical/HGBat.h"
#include "../../global/classical/katsuura.h"
#include "../../global/classical/scaffer_F6.h"
#include "../../global/classical/griewank_rosenbrock.h"
#include "../../global/classical/ackley.h"

namespace ofec {
	namespace CEC2015 {
		F15_global_composition7::F15_global_composition7(const ParamMap &v) :
			F15_global_composition7((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		F15_global_composition7::F15_global_composition7(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {
			
		}

		void F15_global_composition7::setFunction() {
			basic_func f(10);
			f[0] = &create_function<rastrigin>;
			f[1] = &create_function<weierstrass>;
			f[2] = &create_function<happy_cat>;
			f[3] = &create_function<schwefel>;
			f[4] = &create_function<rosenbrock>;
			f[5] = &create_function<HGBat>;
			f[6] = &create_function<katsuura>;
			f[7] = &create_function<scaffer_F6>;
			f[8] = &create_function<griewank_rosenbrock>;
			f[9] = &create_function<ackley>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i]("", m_num_vars, m_num_objs));
				m_function[i]->initialize();
				m_function[i]->setBias(0);
			}

			for (auto &i : m_function)
				i->setConditionNumber(1.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 10;
			m_converge_severity[2] = 20;
			m_converge_severity[3] = 20;
			m_converge_severity[4] = 30;
			m_converge_severity[5] = 30;
			m_converge_severity[6] = 40;
			m_converge_severity[7] = 40;
			m_converge_severity[8] = 50;
			m_converge_severity[9] = 50;


			m_height[0] = 0.1;
			m_height[1] = 2.5e-1;
			m_height[2] = 0.1;
			m_height[3] = 2.5e-2;
			m_height[4] = 1e-3;
			m_height[5] = 0.1;
			m_height[6] = 1e-5;
			m_height[7] = 10;
			m_height[8] = 2.5e-2;
			m_height[9] = 1e-3;

			m_f_bias[0] = 0;
			m_f_bias[1] = 100;
			m_f_bias[2] = 100;
			m_f_bias[3] = 200;
			m_f_bias[4] = 200;
			m_f_bias[5] = 300;
			m_f_bias[6] = 300;
			m_f_bias[7] = 400;
			m_f_bias[8] = 400;
			m_f_bias[9] = 500;

		}

		void F15_global_composition7::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			m_num_function = 10;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
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
		int F15_global_composition7::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
			obj[0] += 1500;
			return kNormalEval;
		}

	}
}