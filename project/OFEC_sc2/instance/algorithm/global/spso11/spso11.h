//Register SPSO11 "SPSO11" ConOP,GOP,MMOP,SOP

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
// Created: 21 September 2011
// Last modified: 21 Nov 2014

/*
SPSO11: C. Maurice, "Standard PSO 2011 (SPSO-2011)" http://www.particleswarm.info/Programs.html, 2011.
 M. Clerc, Standard Particle Swarm Optimisation, Particle Swarm Central, Tech.Rep., 2012,
*/
#ifndef SPSO11_H
#define SPSO11_H

#include "../../../../core/algorithm/algorithm.h"
#include "particle11.h"
#include "../../template/classic/pso/swarm.h"

namespace ofec {
	class SPSO11 final : public Algorithm {
	protected:
		std::unique_ptr<Swarm<Particle11>> m_pop;
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
#endif // SPSO11_H
