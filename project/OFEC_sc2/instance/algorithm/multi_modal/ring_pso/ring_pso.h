//Register RingPSO "Ring-PSO" ConOP,GOP,MMOP,SOP

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
// Modified: 21 Nov 2014
// Modified: 2 Dec 2019 by WangJunChen

/* X. Li, 
"Niching Without Niching Parameters: Particle Swarm Optimization Using a Ring Topology," 
in IEEE Transactions on Evolutionary Computation, vol. 14, no. 1, pp. 150-169, Feb. 2010.
doi: 10.1109/TEVC.2009.2026270 */

#ifndef OFEC_RINGPSO_H
#define OFEC_RINGPSO_H

#include "../../template/classic/pso/swarm.h"
#include "../../template/classic/pso/particle.h"
#include "../../../../core/algorithm/algorithm.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	class RingParticle : public Particle {
	public:
		RingParticle(size_t num_obj, size_t num_con, size_t size_var) : Particle(num_obj, num_con, size_var) {}
		RingParticle(const Solution<> & rhs) : Particle(rhs) {}
		void nextVelocity(const Solution<>* lbest, Real w, Real c1, Real c2, int id_rnd) override;
	};

	class RingSwarm : public Swarm<RingParticle> {
	public:
		enum class Topology { R2, R3, LHC_R2, LHC_R3 };
		RingSwarm(size_t size_pop, Topology topology, int id_pro);
		void setNeighborhood(int id_rnd) override;
	protected:
		bool m_is_neighbor_set;
		Topology m_topology;
	};

	class RingPSO : public Algorithm {
	protected:
		std::unique_ptr<RingSwarm> m_pop;
		size_t m_pop_size;
		Real m_w, m_c1, m_c2;
		RingSwarm::Topology m_topology;

		void initialize_() override;
		void run_() override;
		void initSwarm();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:
		void record() override;
	};
}
#endif // !OFEC_RINGPSO_H
