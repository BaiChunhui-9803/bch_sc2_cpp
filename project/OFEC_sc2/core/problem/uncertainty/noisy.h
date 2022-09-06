/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
*
*********************************************************************************/

// Created: 9 June 2021
// Last modified:

#ifndef OFEC_NOISY_H
#define OFEC_NOISY_H
#include "../problem.h"

namespace ofec {
#define GET_NOISY dynamic_cast<Noisy&>(GET_PRO(id_pro))

	class Noisy : virtual public Problem {
	protected:
		// there are three sources of noisy
		bool m_flag_noisy_from_objective = false;
		Real m_obj_noisy_severity = 0.0;
		bool m_flag_noisy_from_variable = false;
		Real m_var_noisy_severity = 0.0;
		bool m_flag_noisy_from_environment = false;
		Real m_environment_noisy_severity = 0.0;

	public:
		Noisy() = default;
		//template<typename ... Args>
		//Noisy(Args&& ... args) : Problem(std::forward<Args>(args)...) {};
		virtual ~Noisy() = default;
		Noisy& operator=(const Noisy& rhs) = default;

		void setFlagNoisyFromObjective(bool flag_noisy_obj) {
			m_flag_noisy_from_objective = flag_noisy_obj;
		}

		bool getFlagNoisyFromObjective()const {
			return m_flag_noisy_from_objective;
		}
		void setFlagNoisyFromVariable(bool flag_noisy_var) {
			m_flag_noisy_from_variable = flag_noisy_var;
		}

		bool getFlagNoisyFromVariable()const {
			return m_flag_noisy_from_variable;
		}
		void setVarNoisyServerity(Real val) {
			m_var_noisy_severity = val;
		}

		void setFlagNoisyFromEnvironment(bool flag_noisy_env) {
			m_flag_noisy_from_environment = flag_noisy_env;
		}

		bool getFlagNoisyFromEnvironment()const {
			return m_flag_noisy_from_environment;
		}
		virtual void updateParameters() override {}

	protected:
		virtual void initialize_()override;
		virtual void copy(const Problem& rP)override;

	};
}

#endif