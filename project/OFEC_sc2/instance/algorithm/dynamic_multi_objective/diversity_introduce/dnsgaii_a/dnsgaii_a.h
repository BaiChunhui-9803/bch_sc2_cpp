//Register DNSGAII_A "DNSGAII-A" DMOP,MOP,ConOP,DOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Qingshan Tan
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

* see https://github.com/Changhe160/OFEC for more information
*************************************************************************/

/************************************************************************
Deb K, Karthik S.
Dynamic multi-objective optimization and decision-making using modified NSGA-II:
a case study on hydro-thermal power scheduling[C]
International conference on evolutionary multi-criterion optimization. Springer, Berlin, Heidelberg, 2007: 803-817.
************************************************************************/

// Created: 19 Octorber 2019 by Qingshan Tan

#ifndef OFEC_DNSGAII_A_H
#define OFEC_DNSGAII_A_H

#include"../../../template/multi_objective/nsgaii/nsgaii.h"
#include"../../../template/classic/ga/sbx_pop.h"
#include"../../../../../core/algorithm/algorithm.h"
#include"../../../../../core/algorithm/population.h"

namespace ofec {
	class DNSGAII_A_pop : public PopSBX<>, NSGAII<individual<>> {
	public:
		explicit DNSGAII_A_pop(size_t size_pop);
		void initialize() override;
		int evolve() override;
		//std::vector<size_t> get_rand_seq() { return m_rand_seq; }
		bool problem_changed();
		void response_change();
		void update_pop();
		Real get_detect_rate() { return m_detect_rate; }
		Real get_replace_rate() { return m_replace_rate; }
		void set_detect_rate(Real r) { m_detect_rate = r; }
		void set_replace_rate(Real r) { m_replace_rate = r; }
	protected:
		std::vector<individual<>> m_offspring;  // 2 size of population
		Real m_detect_rate;//the ratio of population to re-evaluate
		Real m_replace_rate;//the ratio of population to be replaced
	};

	class DNSGAII_A : public algorithm {
	public:
		explicit DNSGAII_A(param_map& v);
		void initialize() override;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override {}
#endif
	protected:
		void run_() override;
	protected:
		DNSGAII_A_pop m_pop;
	};
}

#endif // !OFEC_DNSGAII_A_H
