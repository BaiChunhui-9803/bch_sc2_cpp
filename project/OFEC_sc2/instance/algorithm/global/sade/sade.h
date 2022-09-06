//Register SaDE "SaDE" GOP,ConOP,SOP,MMOP

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

/*
Paper: A. K. Qin, V. L. Huang and P. N. Suganthan, ��Differential evolution with 
strategy adaptation for global numerical optimization,�� IEEE Transactions on 
Evolutionary Computation, 13(2): 398- 417, 2009. 
*/
#ifndef OFEC_SADE_H
#define OFEC_SADE_H

#include "sade_pop.h"
#include "../../../../core/algorithm/algorithm.h"

namespace ofec {
	class SaDE final : public Algorithm	{
	protected:
		std::unique_ptr<PopSaDE> m_pop;
		size_t m_pop_size;

		void initialize_() override;
		void run_() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:		
		void record() override;
#ifdef OFEC_DEMO
		const std::vector<Real> &ratioStrategy() const { return m_pop->ratioStrategy(); }
#endif
	};
}
#endif // OFEC_SADE_H
