//Register MOEAD_DE "MOEA/D-DE" MOP,ConOP

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
// Last modified: 24 Aug 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)


#ifndef OFEC_MOEAD_DE_H
#define OFEC_MOEAD_DE_H

#include "../../template/multi_objective/moead/moead.h"
#include "../moea_de_pop.h"

namespace ofec {
	class PopMOEAD_DE :public PopMODE<>, MOEAD<IndDE> {
	public:
		PopMOEAD_DE(size_t size_pop, int id_pro);
		void initialize_(int id_pro, int id_rnd);
		int evolve(int id_pro, int id_alg, int id_rnd);
	};

	class MOEAD_DE :public Algorithm {
	public:
		void initialize_() override;
		void record() override;
		void initPop();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif
	protected:
		void run_() override;
		std::unique_ptr<PopMOEAD_DE> m_pop;
		size_t m_pop_size;
	};

}

#endif
