//Register SPSO07 "SPSO07" ConOP,GOP,MMOP,SOP

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
*-------------------------------------------------------------------------------
* SPSO07: C. Maurice, "Standard pso 2007 (spso-07)" http://www.particleswarm.info/Programs.html, 2007.
*
*********************************************************************************/
// Created: 21 September 2011
// Last modified: 21 Nov 2014

#ifndef SPSO07_H
#define SPSO07_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../template/classic/pso/swarm.h"
#include "particle07.h"

namespace ofec {
	class SPSO07 final : public Algorithm {
	protected:
		std::unique_ptr<Swarm<Particle07>> m_pop;
		Real m_w, m_c1, m_c2;
		size_t m_pop_size;

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

#endif // SPSO07_H
