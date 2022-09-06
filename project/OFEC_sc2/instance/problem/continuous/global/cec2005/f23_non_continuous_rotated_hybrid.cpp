//Register CEC2005_GOP_F22 "GOP_CEC2005_F22" GOP,ConOP,SOP

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

#include "F23_non_continuous_rotated_hybrid.h"
#include "../classical/griewank_rosenbrock.h"
#include "../classical/griewank.h"
#include "../classical/rastrigin.h"
#include "../classical/weierstrass.h"
#include "../classical/rotated_scaffer_F6.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	namespace cec2005 {
		void F23_non_continuous_rotated_hybrid::evaluateObjective(Real *x, std::vector<Real> &obj) {
			std::vector<Real> x_(m_num_vars);
			for (size_t j = 0; j < m_num_vars; ++j) {
				Real trans = m_function[0]->translation()[j];
				if (fabs(x[j] - trans) >= 0.5) {
					Real temp_x = 2.0 * x[j];
					int int_part = int(temp_x);
					Real decimal_part = temp_x - int_part;
					if (temp_x <= 0 && decimal_part >= 0.5) temp_x = int_part - 1;
					else if (decimal_part < 0.5) temp_x = int_part;
					else temp_x = int_part + 1;
					x_[j] = temp_x / 2.0;
				}
				else x_[j] = x[j];
			}
			Composition::evaluateObjective(x_.data(), obj);
			obj[0] += 360.;
		}

		void F23_non_continuous_rotated_hybrid::setFunction() {
			m_num_function = 10;
			m_function.resize(m_num_function);
			m_param_ids.resize(m_num_function);
			m_height.resize(m_num_function);
			m_fmax.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_stretch_severity.resize(m_num_function);

			basic_func f(5);
			f[0] = &create_function<RotatedScafferF6>;
			f[1] = &create_function<Rastrigin>;
			f[2] = &create_function<GriewankRosenbrock>;
			f[3] = &create_function<Weierstrass>;
			f[4] = &create_function<Griewank>;

			for (size_t i = 0; i < m_num_function; ++i) {
				auto param = GET_PARAM(m_id_param);
				param["problem name"] = m_name + "_part" + std::to_string(i + 1);
				m_param_ids[i] = ADD_PARAM(param);
				m_function[i] = dynamic_cast<Function*>(f[i / 2]());
				m_function[i]->setIdRnd(m_id_rnd);
				m_function[i]->setIdParam(m_param_ids[i]);
				m_function[i]->initialize();
				m_function[i]->setBias(0);
			}

			for (auto &i : m_function)
				i->setConditionNumber(2.);

			for (int i = 0; i < m_num_function; i++) {
				m_height[i] = 100 * i;
			}

			m_function[0]->setDomain(-100, 100);   m_function[1]->setDomain(-100, 100);
			m_function[2]->setDomain(-5, 5);     m_function[3]->setDomain(-5, 5);
			m_function[4]->setDomain(-5, 5); m_function[5]->setDomain(-5, 5);
			m_function[6]->setDomain(-0.5, 0.5); m_function[7]->setDomain(-0.5, 0.5);
			m_function[8]->setDomain(-200, 200); m_function[9]->setDomain(-200, 200);

			m_stretch_severity[0] = 5.*5. / 100; m_stretch_severity[1] = 5. / 100;
			m_stretch_severity[2] = 5.;		m_stretch_severity[3] = 1.;
			m_stretch_severity[4] = 5;  m_stretch_severity[5] = 1;
			m_stretch_severity[6] = 50.;	m_stretch_severity[7] = 10.;
			m_stretch_severity[8] = 5. * 5 / 200;  m_stretch_severity[9] = 5. / 200;

			m_converge_severity[0] = 1.; m_converge_severity[1] = 1.;
			m_converge_severity[2] = 1.;	m_converge_severity[3] = 1.;
			m_converge_severity[4] = 1.;  m_converge_severity[5] = 2.;
			m_converge_severity[6] = 2.;	m_converge_severity[7] = 2.;
			m_converge_severity[8] = 2.;  m_converge_severity[9] = 2.;

			for (int i = 0; i < m_num_function; i++) {
				m_function[i]->setScale(m_stretch_severity[i]);
			}


			//setBias(360.);
		}
	}
}


