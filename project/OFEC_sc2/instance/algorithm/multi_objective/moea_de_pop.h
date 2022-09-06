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

*  See the details of MOEA/D-DE in the following paper
*  H. Li and Q. Zhang, Comparison Between NSGA-II and MOEA/D on a Set of Multiobjective Optimization
*  Problems with Complicated Pareto Sets, Technical Report CES-476, Department of Computer Science,
*  University of Essex, 2009
*************************************************************************/
// Created: 30 December 2014
// Last modified: 18 July 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)

#ifndef MOEA_DE_POP_H
#define MOEA_DE_POP_H
#include "../template/classic/de/de_ind.h"
#include "../template/classic/de/de_pop.h"
#include <numeric>

namespace ofec {
	template<typename TInd = IndDE>
	class PopMODE : public PopDE<TInd> {
	protected:		
		Real m_mr;
		Real m_meta;
		std::vector<size_t> m_rand_seq; // Random sequence of the population

	public:
		PopMODE(size_t size_pop, int id_pro);
		//PopMODE<TInd>& operator=(const PopMODE& rhs);
		void setParamMODE(Real mr, Real meta) { m_mr = mr; m_meta = meta; }
		void crossMutate(const std::vector<size_t> &index, TInd &child, int id_pro, int id_rnd);
		size_t tournamentSelection(int id_pro, int id_rnd, size_t tournament_size = 2);

	protected:
		void crossover(const TInd &parent1, const TInd &parent2, const TInd &parent3, TInd &child, int id_pro, int id_rnd);
		void mutate(TInd &child, int id_pro, int id_rnd);
	};

	template<typename TInd>
	PopMODE<TInd>::PopMODE(size_t size_pop, int id_pro) : 
		PopDE<TInd>(size_pop, id_pro),
		m_mr(0.),
		m_meta(20) {}

	template<typename TInd>
	void PopMODE<TInd>::crossMutate(const std::vector<size_t> &index, TInd &child, int id_pro, int id_rnd) {
		crossover(this->at(index[0]), this->at(index[1]), this->at(index[2]), child, id_pro, id_rnd);
		mutate(child, id_pro, id_rnd);
	}

	template<typename TInd>
	void PopMODE<TInd>::crossover(const TInd &parent1, const TInd &parent2, const TInd &parent3, TInd &child, int id_pro, int id_rnd) {
		int numDim = GET_CONOP(id_pro).numVariables();
		int idx_rnd = GET_RND(id_rnd).uniform.nextNonStd(0, numDim);
		auto boundary = GET_CONOP(id_pro).domain();
		Real rate = 0.5;
		child = parent1;
		for (int n = 0; n < numDim; n++) {
			/*Selected Two Parents*/
			child.variable()[n] = parent1.variable()[n] + rate * (parent3.variable()[n] - parent2.variable()[n]);
			if (child.variable()[n] < boundary[n].limit.first) {
				Real rnd = GET_RND(id_rnd).uniform.next();
				child.variable()[n] = boundary[n].limit.first + rnd * (parent1.variable()[n] - boundary[n].limit.first);
			}
			if (child.variable()[n] > boundary[n].limit.second) {
				Real rnd = GET_RND(id_rnd).uniform.next();
				child.variable()[n] = boundary[n].limit.second - rnd * (boundary[n].limit.second - parent1.variable()[n]);
			}
		}

	}

	template<typename TInd>
	void PopMODE<TInd>::mutate(TInd &child, int id_pro, int id_rnd) {
		int numDim = GET_CONOP(id_pro).numVariables();
		auto boundary = GET_CONOP(id_pro).domain();
		Real rnd, delta1, delta2, mut_pow, deltaq;
		Real y, yl, yu, val, xy;
		for (int j = 0; j < numDim; j++) {
			if (GET_RND(id_rnd).uniform.next() <= m_mr) {
				y = child.variable()[j];
				yl = boundary[j].limit.first;
				yu = boundary[j].limit.second;
				delta1 = (y - yl) / (yu - yl);
				delta2 = (yu - y) / (yu - yl);
				rnd = GET_RND(id_rnd).uniform.next();
				mut_pow = 1.0 / (m_meta + 1.0);
				if (rnd <= 0.5) {
					xy = 1.0 - delta1;
					val = 2.0 * rnd + (1.0 - 2.0 * rnd) * (pow(xy, (m_meta + 1.0)));
					deltaq = pow(val, mut_pow) - 1.0;
				}
				else {
					xy = 1.0 - delta2;
					val = 2.0 * (1.0 - rnd) + 2.0 * (rnd - 0.5) * (pow(xy, (m_meta + 1.0)));
					deltaq = 1.0 - (pow(val, mut_pow));
				}
				y = y + deltaq * (yu - yl);
				if (y < yl)
					y = yl;
				if (y > yu)
					y = yu;
				child.variable()[j] = y;
			}
		}
	}

	template<typename TInd>
	size_t PopMODE<TInd>::tournamentSelection(int id_pro, int id_rnd, size_t tournament_size) {
		if (m_rand_seq.size() != this->m_inds.size()) {
			m_rand_seq.resize(this->m_inds.size());
			std::iota(m_rand_seq.begin(), m_rand_seq.end(), 0);
		}
		GET_RND(id_rnd).uniform.shuffle(m_rand_seq.begin(), m_rand_seq.end());
		size_t idx_best = m_rand_seq[0];
		for (size_t k = 1; k < tournament_size; ++k)
			if (this->m_inds[m_rand_seq[k]]->dominate(*this->m_inds[idx_best], id_pro))
				idx_best = m_rand_seq[k];
		return idx_best;
	}
}

#endif //MOEA_DE_H