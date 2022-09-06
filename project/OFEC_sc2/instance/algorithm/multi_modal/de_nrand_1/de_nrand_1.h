//Register DE_nrand_1 "DE/nrand/1" ConOP,SOP,GOP,MMOP

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

#ifndef OFEC_DE_NRAND_1_H
#define OFEC_DE_NRAND_1_H

#include "de_nrand_1_pop.h"
#include "../../../../core/algorithm/algorithm.h"

namespace ofec {
	class DE_nrand_1 : public Algorithm {
	protected:
		std::unique_ptr<PopNRand1DE> m_pop;
		size_t m_pop_size;
		Real m_f, m_cr;

		void initialize_() override;
		void run_() override;
		virtual void initPop();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:
		void record() override;
	};
}

#endif // !OFEC_DE_NRAND_1_H

