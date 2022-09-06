//Register ANDE "ANDE" ConOP,SOP,GOP,MMOP

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
*********************************************************************************/
// Created on 20th May, 2020 by Junchen Wang
/*
Z. Wang et al., "Automatic Niching Differential Evolution With Contour Prediction Approach for Multimodal Optimization Problems," 
in IEEE Transactions on Evolutionary Computation, vol. 24, no. 1, pp. 114-128, Feb. 2020, doi: 10.1109/TEVC.2019.2910721.
*/

#ifndef OFEC_ANDE_H
#define OFEC_ANDE_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../template/classic/de/de_pop.h"
#include "../../../../utility/clustering/apc.h"

namespace ofec {
	class ANDE : public Algorithm {
	protected:
		std::unique_ptr<PopDE<>> m_pop;
		std::unique_ptr<APC<IndDE>> m_apc;
		size_t m_MaxFEs = 1000000;
		size_t m_pop_size;
		Real m_f, m_cr;

		void initialize_() override;
		void run_() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:
		void record() override;

	private:
		void CPA(const std::vector<size_t> &cluster);
		void TLLS(const std::vector<std::vector<size_t>>& clusters);
	};
}


#endif // !OFEC_ANDE_H

