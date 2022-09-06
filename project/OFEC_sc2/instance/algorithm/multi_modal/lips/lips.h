//Register LIPS "LIPS" ConOP,GOP,MMOP,SOP

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
// Created at 21 January 2017
// Modifed by WangJunChen at 2 Dec 2019 

/* ---------------------------------------------------------------------------------------
LIPS: B. Y. Qu, P. N. Suganthan and S. Das, 
"A Distance-Based Locally Informed Particle Swarm Model for Multimodal Optimization," 
in IEEE Transactions on Evolutionary Computation, vol. 17, no. 3, pp. 387-402, June 2013.
doi: 10.1109/TEVC.2012.2203138
-----------------------------------------------------------------------------------------*/

#ifndef OFEC_LIPS_H
#define OFEC_LIPS_H

#include "../../../../core/algorithm/algorithm.h"
#include "lips_pop.h"

namespace ofec {
	class LIPS : public Algorithm {
	protected:
		std::unique_ptr<SwarmLIP> m_pop;
		size_t m_pop_size, m_max_evals;

		void initialize_() override;
		void run_() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:		
		void record() override;
	};
}

#endif // !OFEC_LIPS_H

