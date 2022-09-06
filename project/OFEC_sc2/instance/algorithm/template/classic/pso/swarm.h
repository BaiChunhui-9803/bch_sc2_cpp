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
* class Swarm for TParticle swarm optimization
*
*
*********************************************************************************/
// Created: 21 Sep. 2011 by Changhe Li
// Updated: 12 Dec. 2014 by Changhe Li
// Updated  15 Mar. 2018 by Junchen Wang
// Updated: 1 Sep. 2018 by Changhe Li
// Updated: 25.Aug. 2019 Changhe Li

#ifndef SWARM_H
#define SWARM_H

#include "../../../../../core/algorithm/population.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/problem/solution.h"
#include <numeric>

namespace ofec {
	template <typename TParticle>
	class Swarm : public Population<TParticle> {
	protected:			
		Real m_C1 = 1.496, m_C2 = 1.496;						//accelerators
		Real m_W = 0.7298;								// inertia weight		
		std::vector<std::vector<bool>> m_link;			//population topology
		bool m_flag_best_impr = false;
	public:
		Swarm(size_t size_pop, int id_pro);
		void initPbest(int id_pro);
		void initVelocity(int id_pro, int id_rnd);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
		Real& W() { return m_W; }
		Real& C1() { return m_C1; }
		Real& C2() { return m_C2; }
		Real averageSpeed() const;
		virtual void setNeighborhood(int id_rnd); //  The adaptive random topology by default
		virtual Solution<>& neighborhoodBest(int idx, int id_pro);
	};
	
	template<typename TParticle>
	Swarm<TParticle>::Swarm(size_t size_pop, int id_pro) : 
		Population<TParticle>(size_pop, id_pro, GET_CONOP(id_pro).numVariables()),
		m_link(size_pop, std::vector<bool>(size_pop, false)) {}

	template<typename TParticle>
	void Swarm<TParticle>::initPbest(int id_pro) {
		for (auto& i : this->m_inds)
			i->pbest() = i->solut();
		this->updateBest(id_pro);
	}

	template<typename TParticle>
	void Swarm<TParticle>::initVelocity(int id_pro, int id_rnd) {
		for (auto &i : this->m_inds)
			i->initVelocity(id_pro, id_rnd);
	}

	template<typename TParticle>
	int Swarm<TParticle>::evolve(int id_pro, int id_alg, int id_rnd)	{
		int rf = kNormalEval;
		//generate a permutation of particle index
		std::vector<int> rindex(this->m_inds.size());
		std::iota(rindex.begin(), rindex.end(), 0);
		GET_RND(id_rnd).uniform.shuffle(rindex.begin(), rindex.end());

		this->setNeighborhood(id_rnd);

		bool flag = false;
		for (int i = 0; i < this->m_inds.size(); i++) {
			auto& x = neighborhoodBest(rindex[i], id_pro);

			this->m_inds[rindex[i]]->nextVelocity(&x, m_W, m_C1, m_C2, id_rnd);
			this->m_inds[rindex[i]]->move();
			this->m_inds[rindex[i]]->clampVelocity(id_pro, id_rnd);

			rf = this->m_inds[rindex[i]]->evaluate(id_pro, id_alg);

			if (this->m_inds[rindex[i]]->dominate(this->m_inds[rindex[i]]->pbest(), GET_PRO(id_pro).optMode())) {
				this->m_inds[rindex[i]]->pbest() = this->m_inds[rindex[i]]->solut();
				if (this->updateBest(this->m_inds[rindex[i]]->solut(), id_pro))
					flag = true;
			}

			if (rf != kNormalEval) break;
		}
		m_flag_best_impr = flag;
		this->m_iter++;
		return rf;
	}

	template<typename TParticle>
	Real Swarm<TParticle>::averageSpeed() const {
		Real avg = 0;		
		for (int i = 0; i<this->m_inds.size(); ++i) 
			avg += this->m_inds[i]->speed();
		return avg > 0 ? avg / this->m_inds.size() : 0;
	}

	template<typename TParticle>
	Solution<>& Swarm<TParticle>::neighborhoodBest(int idx, int id_pro) {
		int l = idx;
		for (int i = 0; i < this->m_inds.size();++i) {
			if (m_link[idx][i]&&this->m_inds[i]->pbest().dominate(this->m_inds[l]->pbest(), GET_PRO(id_pro).optMode()))
				l = i;
		}
		return this->m_inds[l]->pbest();
	}

	template<typename TParticle>
	void Swarm<TParticle>::setNeighborhood(int id_rnd) {
		if (m_link.size() != this->m_inds.size()) {
			m_link.assign(this->m_inds.size(), std::vector<bool>(this->m_inds.size()));
			m_flag_best_impr = false;
		}
		if (!m_flag_best_impr) {
			Real p = 1 - pow(1 - 1. / this->m_inds.size(), 3);
			for (int i = 0; i<this->m_inds.size(); i++) {
				for (int k = 0; k<this->m_inds.size(); k++) {
					if (k == i || GET_RND(id_rnd).uniform.next() < p)
						m_link[i][k] = true; //probabilistic method
					else
						m_link[i][k] = false;
				}
			}
		}
	}

}
#endif // !SWARM_H


