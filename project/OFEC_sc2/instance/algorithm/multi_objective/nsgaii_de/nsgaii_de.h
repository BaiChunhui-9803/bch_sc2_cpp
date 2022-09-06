//Register NSGAII_DE "NSGAII-DE" ConOP,MOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 13 Jan 2015
// Last modified: 3 May 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)

#ifndef OFEC_NSGAII_DE_H
#define OFEC_NSGAII_DE_H

#include "../../template/multi_objective/nsgaii/nsgaii.h"
#include "../moea_de_pop.h"

namespace ofec {
	class PopNSGAII_DE : public PopMODE<>, NSGAII {
	public:
		PopNSGAII_DE(size_t size_pop, int id_pro);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
	protected:
		Population<IndDE> m_pop_combined; // combination of parent and children
	};

	class NSGAII_DE : public Algorithm {
	public:
		void initialize_() override;
		void record() override;
		void initPop();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif
	protected:
		void run_() override;
	protected:
		std::unique_ptr<PopNSGAII_DE> m_pop;
		size_t m_pop_size;
		Real m_mr, m_meta;
	};
}

#endif // !OFEC_NSGAII_DE_H
