/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#include "F13_expensive_composition1.h"
#include "../../global/classical/rotated_rosenbrock.h"
#include "../../global/classical/elliptic.h"
#include "../../global/classical/rotated_bent_cigar.h"
#include "../../global/classical/rotated_discus.h"
#include "../../global/classical/elliptic.h"





namespace ofec {
	namespace CEC2015 {
		F13_expensive_composition1::F13_expensive_composition1(const ParamMap &v) :
			F13_expensive_composition1((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		F13_expensive_composition1::F13_expensive_composition1(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition_2015(name, size_var, size_obj) {

			
		}
		
		void F13_expensive_composition1::setFunction() {
			basic_func f(5);
			f[0] = &create_function<rotated_rosenbrock>;
			f[1] = &create_function<elliptic>;
			f[2] = &create_function<rotated_bent_cigar>;
			f[3] = &create_function<rotated_discus>;
			f[4] = &create_function<elliptic>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i]("", m_num_vars, m_num_objs));
				m_function[i]->initialize();
				m_function[i]->setBias(0);
			}

			for (auto &i : m_function)
				i->setConditionNumber(2.);

			m_converge_severity[0] = 10;
			m_converge_severity[1] = 20;
			m_converge_severity[2] = 30;
			m_converge_severity[3] = 40;
			m_converge_severity[4] = 50;

			m_height[0] = 1;
			m_height[1] = 1e-6;
			m_height[2] = 1e-26;
			m_height[3] = 1e-6;
			m_height[4] = 1e-6;

			
			m_f_bias[0] = 0;
			m_f_bias[1] = 100;
			m_f_bias[2] = 200;
			m_f_bias[3] = 300;
			m_f_bias[4] = 400;

			//setBias(1300);
		}
		void F13_expensive_composition1::initialize() {
			m_num_function = 5;
			m_function.resize(m_num_function);
			m_height.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_f_bias.resize(m_num_function);
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			m_variable_monitor = true;

			setFunction();
			loadTranslation("/instance/problem/continuous/expensive/CEC2015/data/");  //data path
			
			for (auto &i : m_function) {
				i->get_optima().clear();
				i->setGlobalOpt(i->translation().data());
			}
			// Set optimal solution
            Solution<VarVec<Real>, Real> s(m_num_objs, num_constraints(), m_num_vars);
			for (int i = 0; i < m_num_vars; ++i) {
				s.variable()[i] = m_function[0]->get_optima().variable(0)[i];
			}
			m_optima.append(s.variable());

			s.evaluate(false, caller::Problem);
			m_optima.append(s.objective());
			m_initialized = true;
		}

		int F13_expensive_composition1::evaluateObjective(Real *x, std::vector<Real> &obj) {
			composition_2015::evaluateObjective(x, obj);
			obj[0] += 1300;
			return kNormalEval;
		}

	}
}


