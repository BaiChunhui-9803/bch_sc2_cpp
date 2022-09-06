//Register NCDE "NCDE" GOP,ConOP,SOP,MMOP

/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou
// Updated on 15th Aug, 2019 by Junchen Wang


#ifndef OFEC_NCDE_H
#define OFEC_NCDE_H

/*
Paper:B. Y. Qu, P. N. Suganthan, and J. J. Liang, "Differential evolution
with neighborhood mutation for multimodal optimization", IEEE Trans.
Evol. Comput., vol. 16, no. 5, pp. 601�C614, Oct. 2012.
*/

#include "../../template/classic/de/de_pop.h"
#include "../../../../core/algorithm/algorithm.h"
#include <list>

namespace ofec {
	class PopNCDE final : public PopDE<> {
	public:
		PopNCDE(size_t size_pop, int id_pro);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
	protected:
		void sortDistance(size_t a, int id_pro);
	protected:
		std::vector<std::list<std::pair<Real, int>>> m_dis;  //save individuals' distance
		Real m_r;                             //size of neighborhood
		size_t m_m;                                //size of neighborhood
	};

	class NCDE final : public Algorithm {
	protected:
		std::unique_ptr<PopNCDE> m_pop;
		size_t m_pop_size;
		Real m_f, m_cr;
		MutationDE m_ms;

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


#endif // OFEC_NCDE_H

