//Register NSGAIII_SBX "NSGAIII-SBX" ConOP,MOP

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

#ifndef OFEC_NSGAIII_SBX_H
#define OFEC_NSGAIII_SBX_H

#include "../../template/multi_objective/nsgaiii/nsgaiii.h"
#include "../../../../core/algorithm/individual.h"
#include "../../template/classic/ga/sbx_pop.h"

namespace ofec {
	class PopNSGAIII_SBX : public PopSBX<>, NSGAIII<Individual<>> {
	public:
		explicit PopNSGAIII_SBX(size_t size_pop, int id_pro, size_t size_var, size_t size_obj, const std::vector<OptMode>& opt_mode);
		void initialize_(int id_pro, int id_rnd);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
	protected:
		std::vector<Individual<>> m_offspring;  // 2 size of population
	};

	class NSGAIII_SBX : public Algorithm {
	public:
		void initialize_() override;
		void record() override;
		void initPop();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif
	protected:
		void run_() override;
		std::unique_ptr<PopNSGAIII_SBX> m_pop;
		size_t m_pop_size;
	};
}

#endif // !OFEC_NSGAIII_SBX_H

