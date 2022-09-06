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
*-------------------------------------------------------------------------------
* class DEpopulation manages a set of DEindividuals.
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou

/*Storn, R. and Price, K. (1997), "Differential Evolution - A Simple and Efficient Heuristic for Global Optimization over Continuous Spaces",
Journal of Global Optimization, 11, pp. 341-359*/

#ifndef DE_POPULATION_H
#define DE_POPULATION_H

#include "de_ind.h"
#include "../../../../../core/algorithm/population.h"
#include "../../../../../core/problem/continuous/continuous.h"

namespace ofec {
	enum class MutationDE { rand_1, best_1, current_to_best_1, best_2, rand_2, rand_to_best_1, current_to_rand_1 };
	template<typename TInd = IndDE>
	class PopDE : public Population<TInd> {
	protected:
		Real m_F, m_CR;
		MutationDE m_mutation_strategy;
		RecombineDE m_recombine_strategy;
	public:
		using Population<TInd>::m_inds;
		using Population<TInd>::m_best;
		using Population<TInd>::m_iter;
		using Population<TInd>::updateBest;

		PopDE();
		PopDE(size_t size_pop, int id_pro);
		PopDE(const PopDE& rhs);
		PopDE(PopDE&& rhs) noexcept;
	//	void resize(size_t size_pop, int id_pro);

	//	void resize(size_t size, int id_pro);

		PopDE& operator=(const PopDE& rhs);
		PopDE& operator=(PopDE&& rhs) noexcept;

		Real& F() { return m_F; }
		Real& CR() { return m_CR; }
		MutationDE& mutationStrategy() { return m_mutation_strategy; }
		RecombineDE& recombineStrategy() { return m_recombine_strategy; }
		void setParameter(const Real cr, const Real f);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
		virtual void mutate(int idx, int id_rnd, int id_pro);
		void mutate(int idx, const std::vector<int>& var, int id_rnd, int id_pro);
		void recombine(int idx, int id_rnd, int id_pro);
		virtual void select(int base_, int number, std::vector<size_t>& result, int id_rnd);
		virtual void selectInNeighborhood(int number, std::vector<size_t>, std::vector<size_t>&, int id_rnd);
	};

	template<typename TInd>
	PopDE<TInd>::PopDE() :
		m_F(0.5),
		m_CR(0.6),
		m_mutation_strategy(MutationDE::rand_1),
		m_recombine_strategy(RecombineDE::Binomial)	{}

	template<typename TInd>
	PopDE<TInd>::PopDE(size_t size_pop, int id_pro) :
		Population<TInd>(size_pop, id_pro, GET_CONOP(id_pro).numVariables()),
		m_F(0.5),
		m_CR(0.6),
		m_mutation_strategy(MutationDE::rand_1),
		m_recombine_strategy(RecombineDE::Binomial) {}

	template<typename TInd>
	PopDE<TInd>::PopDE(const PopDE& rhs) :
		Population<TInd>(rhs),
		m_F(rhs.m_F),
		m_CR(rhs.m_CR),
		m_mutation_strategy(rhs.m_mutation_strategy),
		m_recombine_strategy(rhs.m_recombine_strategy) {}

	template<typename TInd>
	PopDE<TInd>::PopDE(PopDE&& rhs) noexcept :
		Population<TInd>(std::move(rhs)),
		m_F(rhs.m_F),
		m_CR(rhs.m_CR),
		m_mutation_strategy(rhs.m_mutation_strategy),
		m_recombine_strategy(rhs.m_recombine_strategy) {}

	template<typename IndType>
	PopDE<IndType>& PopDE<IndType>::operator=(const PopDE& rhs) {
		if (this == &rhs) return *this;
		Population<TInd>::operator=(rhs);
		m_CR = rhs.m_CR;
		m_F = rhs.m_F;
		m_mutation_strategy = rhs.m_mutation_strategy;
		m_recombine_strategy = rhs.m_recombine_strategy;
		return *this;
	}

	template<typename TInd>
	PopDE<TInd>& PopDE<TInd>::operator=(PopDE&& rhs) noexcept {
		if (this == &rhs) return *this;
		Population<TInd>::operator=(std::move(rhs));
		m_CR = rhs.m_CR;
		m_F = rhs.m_F;
		m_mutation_strategy = rhs.m_mutation_strategy;
		m_recombine_strategy = rhs.m_recombine_strategy;
		return *this;
	}

	template<typename TInd>
	void PopDE<TInd>::mutate(const int idx, int id_rnd, int id_pro) {
		std::vector<size_t> ridx;
		switch (m_mutation_strategy) {
		case MutationDE::rand_1:
			select(idx, 3, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro);
			break;
		case MutationDE::best_1:
			select(idx, 2, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, m_best.front().get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), id_pro);
			break;
		case MutationDE::current_to_best_1:
			select(idx, 2, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, m_inds[idx].get(), m_best.front().get(), m_inds[idx].get(), id_pro, m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
			break;
		case MutationDE::best_2:
			select(idx, 4, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, m_best.front().get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), id_pro, m_inds[ridx[2]].get(), m_inds[ridx[3]].get());
			break;
		case MutationDE::rand_2:
			select(idx, 5, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro, m_inds[ridx[3]].get(), m_inds[ridx[4]].get());
			break;
		case MutationDE::rand_to_best_1:
			select(idx, 3, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, m_inds[ridx[0]].get(), m_best.front().get(), m_inds[ridx[0]].get(), id_pro, m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		case MutationDE::current_to_rand_1:
			select(idx, 3, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[idx].get(), id_pro, m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		}
	}

	template<typename TInd>
	void PopDE<TInd>::recombine(int idx, int id_rnd, int id_pro) {
		m_inds[idx]->recombine(m_CR, m_recombine_strategy, id_rnd, id_pro);
	}

	template<typename TInd>
	void PopDE<TInd>::mutate(int idx, const std::vector<int>& var, int id_rnd, int id_pro) {
		std::vector<size_t> ridx;
		switch (m_mutation_strategy) {
		case MutationDE::rand_1:
			select(idx, 3, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, var, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro);
			break;
		case MutationDE::best_1:
			select(idx, 2, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, var, m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), id_pro);
			break;
		case MutationDE::current_to_best_1:
			select(idx, 2, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, var, m_inds[idx].get(), m_best[0].get(), m_inds[idx].get(), id_pro, m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
			break;
		case MutationDE::best_2:
			select(idx, 4, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, var, m_best[0].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), id_pro, m_inds[ridx[2]].get(), m_inds[ridx[3]].get());
			break;
		case MutationDE::rand_2:
			select(idx, 5, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, var, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro, m_inds[ridx[3]].get(), m_inds[ridx[4]].get());
			break;
		case MutationDE::rand_to_best_1:
			select(idx, 3, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, var, m_inds[ridx[0]].get(), m_best[0].get(), m_inds[ridx[0]].get(), id_pro, m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		case MutationDE::current_to_rand_1:
			select(idx, 3, ridx, id_rnd);
			m_inds[idx]->mutate(m_F, var, m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[idx].get(), id_pro, m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		}
	}

	template<typename TInd>
	void PopDE<TInd>::setParameter(const Real cr, const Real f) {
		m_CR = cr;
		m_F = f;
	}
	template<typename TInd>
	int PopDE<TInd>::evolve(int id_pro, int id_alg, int id_rnd) {
		if (m_inds.size() < 5) {
			throw MyExcept("the population size cannot be smaller than 5@DE::PopDE<TInd>::evolve()");
		}
		int tag = kNormalEval;
		if (m_mutation_strategy == MutationDE::best_1 ||
			m_mutation_strategy == MutationDE::current_to_best_1 ||
			m_mutation_strategy == MutationDE::best_2 ||
			m_mutation_strategy == MutationDE::rand_to_best_1)
			updateBest(id_pro);
		for (size_t i = 0; i < m_inds.size(); ++i) {
			mutate(i, id_rnd, id_pro);
			recombine(i, id_rnd, id_pro);
			tag = m_inds[i]->select(id_pro, id_alg);
			if (tag & kTerminate) break;
		}
		if (tag == kNormalEval)
			m_iter++;
		return tag;
	}

	template<typename TInd>
	void PopDE<TInd>::select(int base_, int number, std::vector<size_t>& result, int id_rnd) {
		std::vector<int> candidate;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			if (base_ != i) candidate.push_back(i);
		}
		if (result.size() != number)	result.resize(number);
		for (size_t i = 0; i < number; ++i) {
			size_t idx = GET_RND(id_rnd).uniform.nextNonStd<size_t>(0, candidate.size() - i);
			result[i] = candidate[idx];
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}

	template<typename TInd>
	void PopDE<TInd>::selectInNeighborhood(int number, std::vector<size_t> candidate, std::vector<size_t>& result, int id_rnd) {
		if (result.size() != number)	result.resize(number);
		for (size_t i = 0; i < number; ++i) {
			int idx = GET_RND(id_rnd).uniform.nextNonStd<int>(0, (int)candidate.size() - i);
			result[i] = candidate[idx];
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}
}
#endif // !DE_POPULATION_H
