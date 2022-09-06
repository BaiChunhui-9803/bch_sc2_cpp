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
#ifndef OFEC_DYNAMIC_H
#define OFEC_DYNAMIC_H

#include"../problem.h"

namespace ofec {
#define GET_DOP(id_pro) dynamic_cast<Dynamic&>(GET_PRO(id_pro))
	class Dynamic : virtual public Problem {
	protected:
		int m_frequency = 0;
		int m_period = 0;					// definite period for values repeating
		int m_counter = 0;			// counter of number of changes
		size_t m_time_latest_changed = 0;
		// flag=true, the number of dimensions of the problem change, otherwise no change,  default value is false
		bool m_flag_variable_memory_change = false;
		// dir=1 means increasing the memory, dir=-1 decrease it, dir=0 random dir
		int m_flag_variable_memory_dir = 0;
		bool m_flag_objective_memory_change = false;
		bool m_flag_objective_memory_dir = 0;
		bool m_flag_time_linkage = false;
		// severity of noise and time-linkage added in time-linkage enviroment, June 05 2014
		Real m_time_linkage_severity = 0.0;
		bool m_flag_trigger_time_linkage = false;
		bool m_flag_change_during_execution = false;

	protected:
		void updateCurState(int id_alg, bool effective_eval) override;

	public:
		Dynamic() = default;
		virtual ~Dynamic() = default;
		Dynamic &operator=(const Dynamic &rhs) = default;
		void setFrequency(int rChangeFre) { m_frequency = rChangeFre; }
		int getFrequency()const { return m_frequency; }
		void setPeriod(int rPeriod) { m_period = rPeriod; }
		int getPeriod() const { return m_period; }
		void setCount(int counter) { m_counter = counter; }
		int getCount()const { return m_counter; }
		void setFlagVarMemChange(bool flag) { m_flag_variable_memory_change = flag; }
		bool getFlagVarMemChange()const { return m_flag_variable_memory_change; }
		void setFlagTimeLinkage(bool flag) { m_flag_time_linkage = flag; }
		bool getFlagTimeLinkage() const { return m_flag_time_linkage; }
		void setTimeLinkageSeverity(Real value) { m_time_linkage_severity = value; }
		bool &getTriggerFlagTimeLinkage() { return m_flag_trigger_time_linkage; }
		virtual void changeVarMemory() {};
		virtual void change() { ++m_counter; }
		virtual void updateParameters() override;
		virtual int updateEvalTag(SolBase &s, int id_alg, bool effective_eval) override;

	protected:
		virtual void initialize_()override;
		virtual void copy(const Problem &rP) override;

	};
}

#endif