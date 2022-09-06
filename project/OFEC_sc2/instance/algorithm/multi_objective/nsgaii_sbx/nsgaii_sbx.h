//Register NSGAII_SBX "NSGAII-SBX" MOP,ConOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia & Junchen Wang
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 7 Jan 2015
// Last modified: 15 Aug 2019 by Junchen Wang (email:wangjunchen.chris@gmail.com)
// Last modified: 23 Aug 2019 by Xiaofang Wu (email:email:wuxiaofang@cug.edu.cn)

#ifndef OFEC_NSGAII_SBX_H
#define OFEC_NSGAII_SBX_H

#include "../../template/classic/ga/sbx_pop.h"
#include "../../template/multi_objective/nsgaii/nsgaii.h"
#include "../../../../core/algorithm/algorithm.h"

namespace ofec {
	class PopNSGAII_SBX : public PopSBX<>, public NSGAII {
	public:
		PopNSGAII_SBX(size_t size_pop, int id_pro);
		int evolve(int ind_pro, int id_alg, int id_rnd) override;
	protected:
		Population<Individual<>> m_pop_combined;  // combination of parent and children
	};

	class NSGAII_SBX : public Algorithm {
	protected:
		std::unique_ptr<PopNSGAII_SBX> m_pop;
		size_t m_pop_size;
		Real m_cr, m_mr, m_ceta, m_meta;
		
		void initialize_() override;
		void run_() override;
		void initPop();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:
		void record() override;
	};
}

#endif // !OFEC_NSGAII_SBX_H

