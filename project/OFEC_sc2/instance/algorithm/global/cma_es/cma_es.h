//Register CMA_ES "CMA-ES" ConOP,GOP,SOP,MMOP

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
// Created by Junchen Wang on Oct. 29, 2018.

/*Hansen, N. and A.Ostermeier(2001).
Completely Derandomized Self - Adaptation in Evolution Strategies.
Evolutionary Computation, 9(2), pp. 159 - 195;
https://github.com/CMA-ES/c-cmaes */

#ifndef OFEC_CMAES_H
#define OFEC_CMAES_H

#include "../../../../core/algorithm/algorithm.h"
#include "cmaes_pop.h"

namespace ofec {
	class CMA_ES : public Algorithm {
	protected:
		std::unique_ptr<PopCMAES>  m_pop;
		size_t m_pop_size;

		void run_() override;
		void initialize_() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:
		void record() override;
	};
}

#endif // !OFEC_CMAES_H

