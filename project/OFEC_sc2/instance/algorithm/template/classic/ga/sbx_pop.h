/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 7 Jan 2015
// Last modified: 7 Oct 2018 by Junchen Wang (email:wangjunchen.chris@gmail.com)

/*-----------------------------------------------------------------------------------
   SimulatedBinaryCrossover : simulated binary crossover(SBX)
   The implementation was adapted from the code of function realcross() in crossover.c
   http://www.iitk.ac.in/kangal/codes/nsga2/nsga2-gnuplot-v1.1.6.tar.gz
   ref: http://www.slideshare.net/paskorn/simulated-binary-crossover-presentation#
-----------------------------------------------------------------------------------*/

#ifndef OFEC_SBX_POP_H
#define OFEC_SBX_POP_H

#include "ga_pop.h"
#include "../../../../../core/algorithm/individual.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include <algorithm>

namespace ofec {
	template<typename TInd = Individual<>>
	class PopSBX : public PopGA<TInd> {

	protected:
		Real m_ceta; // crossover eta
		Real m_meta; // mutation eta
	public:
		PopSBX(size_t size_pop, int id_pro);
		void setEta(Real ceta, Real meta) { m_ceta = ceta; m_meta = meta; }
		void crossover(size_t idx_parent1, size_t idx_parent2, TInd &child1, TInd &child2, int id_pro, int id_rnd) override;
		void mutate(TInd &indv, int id_pro, int id_rnd) override;
	private:
		Real getBetaq(Real rand, Real alpha);
	};

	template<typename TInd>
	PopSBX<TInd>::PopSBX(size_t size_pop, int id_pro) :
		PopGA<TInd>(size_pop, id_pro, GET_CONOP(id_pro).numVariables()),
		m_ceta(20),
		m_meta(20) {}

	template<typename TInd>
	void PopSBX<TInd>::crossover(size_t idx_parent1, size_t idx_parent2, TInd &child1, TInd &child2, int id_pro, int id_rnd) {
		child1 = *this->m_inds[idx_parent1];
		child2 = *this->m_inds[idx_parent2];
		if (GET_RND(id_rnd).uniform.next() > this->m_cr) return; // not crossovered
		VarVec<Real> &c1 = child1.variable(), &c2 = child2.variable();
		const VarVec<Real> &p1 = this->m_inds[idx_parent1]->variable(), &p2 = this->m_inds[idx_parent2]->variable();
		for (size_t i = 0; i < c1.size(); i += 1) {
			if (GET_RND(id_rnd).uniform.next() > 0.5) continue; // these two variables are not crossovered
			if (std::fabs(static_cast<Real>(p1[i]) - static_cast<Real>(p2[i])) <= 1.0e-14) continue; // two values are the same
			Real y1 = std::min(p1[i], p2[i]), y2 = std::max(p1[i], p2[i]);
			Real lb = GET_CONOP(id_pro).range(i).first, ub = GET_CONOP(id_pro).range(i).second;
			Real rand = GET_RND(id_rnd).uniform.next();
			// child 1
			Real beta = 1.0 + (2.0 * (y1 - lb) / (y2 - y1)),
				alpha = 2.0 - pow(beta, -(m_ceta + 1.0));
			Real betaq = getBetaq(rand, alpha);
			c1[i] = 0.5 * ((y1 + y2) - betaq * (y2 - y1));
			// child 2
			beta = 1.0 + (2.0 * (ub - y2) / (y2 - y1));
			alpha = 2.0 - pow(beta, -(m_ceta + 1.0));
			betaq = getBetaq(rand, alpha);
			c2[i] = 0.5 * ((y1 + y2) + betaq * (y2 - y1));
			// boundary checking
			c1[i] = std::min(ub, std::max(lb, static_cast<Real>(c1[i])));
			c2[i] = std::min(ub, std::max(lb, static_cast<Real>(c2[i])));
			if (GET_RND(id_rnd).uniform.next() <= 0.5)
				std::swap(c1[i], c2[i]);
		}
	}

	template<typename TInd>
	void PopSBX<TInd>::mutate(TInd &indv, int id_pro, int id_rnd) {
		VarVec<Real> &x = indv.variable();
		for (size_t i = 0; i < x.size(); i += 1) {
			if (GET_RND(id_rnd).uniform.next() <= this->m_mr) {
				Real y = indv.variable()[i];
				Real lb = GET_CONOP(id_pro).range(i).first, ub = GET_CONOP(id_pro).range(i).second;
				Real delta1 = (y - lb) / (ub - lb), delta2 = (ub - y) / (ub - lb);
				Real mut_pow = 1.0 / (m_meta + 1.0);
				Real rnd = GET_RND(id_rnd).uniform.next(), deltaq;
				if (rnd <= 0.5) {
					Real xy = 1.0 - delta1;
					Real val = 2.0 * rnd + (1.0 - 2.0 * rnd) * (pow(xy, (m_meta + 1.0)));
					deltaq = pow(val, mut_pow) - 1.0;
				}
				else {
					Real xy = 1.0 - delta2;
					Real val = 2.0 * (1.0 - rnd) + 2.0 * (rnd - 0.5) * (pow(xy, (m_meta + 1.0)));
					deltaq = 1.0 - (pow(val, mut_pow));
				}
				y = y + deltaq * (ub - lb);
				y = std::min(ub, std::max(lb, y));
				x[i] = y;
			}
		}
	}

	template<typename TInd>
	Real PopSBX<TInd>::getBetaq(Real rand, Real alpha) {
		Real betaq;
		if (rand <= (1.0 / alpha))
			betaq = std::pow((rand * alpha), (1.0 / (m_ceta + 1.0)));
		else
			betaq = std::pow((1.0 / (2.0 - rand * alpha)), (1.0 / (m_ceta + 1.0)));
		return betaq;
	}
}

#endif // !OFEC_SBX_POP_H
