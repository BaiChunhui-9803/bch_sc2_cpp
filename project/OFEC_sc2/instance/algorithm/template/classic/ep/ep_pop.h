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
// Created by Junchen Wang on Oct. 28, 2018.

#ifndef OFEC_EP_POP_H
#define OFEC_EP_POP_H

#include "../../../../../core/algorithm/population.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "ep_ind.h"
#include <numeric>

namespace ofec {
	template<typename TInd = IndEP>
	class PopEP : public Population<TInd> {
	public:
		using Population<TInd>::m_inds;
		PopEP() = default;
		PopEP(size_t size_pop, int id_pro);
		PopEP(const PopEP &rhs);
		PopEP(PopEP &&rhs) noexcept;
		PopEP &operator=(const PopEP &rhs);
		PopEP &operator=(PopEP &&rhs) noexcept;
		void resize(size_t size, int id_pro);
		void initialize(int id_pro, int id_rnd) override;
		int evolve(int id_pro, int id_alg, int id_rnd) override;
		Real &tau() { return m_tau; }
		Real &tauPrime() { return m_tau_prime; }
		size_t &q() { return m_q; }
	protected:
		virtual void mutate(int id_rnd, int id_pro);
		virtual void select(int id_rnd, int id_pro);
		void resizeOffspring(int id_pro);
	protected:
		std::vector<std::unique_ptr<TInd>> m_offspring;
		Real m_tau = 0, m_tau_prime = 0;
		size_t m_q = 0;
	};

	template<typename TInd>
	PopEP<TInd>::PopEP(size_t size_pop, int id_pro) : 
		Population<TInd>(size_pop, GET_CONOP(id_pro).numVariables())
	{
		resizeOffspring(id_pro);
	}

	template<typename TInd>
	PopEP<TInd>::PopEP(const PopEP &rhs) : Population<TInd>(rhs) {
		for (size_t i = 0; i < rhs.m_offspring.size(); ++i)
			m_offspring.emplace_back(new TInd(*rhs.m_offspring[i]));
	}

	template<typename TInd>
	PopEP<TInd>::PopEP(PopEP &&rhs) noexcept : 
		Population<TInd>(std::move(rhs)),
		m_offspring(std::move(rhs.m_offspring)) {}

	template<typename TInd>
	PopEP<TInd> &PopEP<TInd>::operator=(const PopEP &rhs) {
		if (this == &rhs) return *this;
		Population<TInd>::operator=(rhs);
		m_offspring.resize(rhs.m_offspring.size());
		for (size_t i = 0; i < rhs.m_offspring.size(); ++i)
			m_offspring[i].reset(new TInd(*rhs.m_offspring[i]));
		return *this;
	}

	template<typename TInd>
	PopEP<TInd> &PopEP<TInd>::operator=(PopEP &&rhs) noexcept {
		if (this == &rhs) return *this;
		Population<TInd>::operator=(std::move(rhs));
		m_offspring = std::move(rhs.m_offspring);
		return *this;
	}

	template<typename TInd>
	void PopEP<TInd>::resize(size_t size, int id_pro) {
		Population<TInd>::resize(size, id_pro, GET_CONOP(id_pro).numVariables());
	}

	template<typename TInd>
	void PopEP<TInd>::initialize(int id_pro, int id_rnd) {
		Population<TInd>::initialize(id_pro, id_rnd);
		for (auto &ind : m_inds)
			ind->initializeEta(id_pro);
	}

	template<typename TInd>
	void PopEP<TInd>::mutate(int id_rnd, int id_pro) {
		for (size_t i = 0; i < m_inds.size(); ++i)
			*m_offspring[i] = *m_inds[i];
		for (size_t i = 0; i < m_inds.size(); i++) {
			Real N = GET_RND(id_rnd).normal.next();
			for (size_t j = 0; j < m_offspring[i]->variable().size(); ++j) {
				m_offspring[i + m_inds.size()]->variable()[j] = m_inds[i]->variable()[j] + m_inds[i]->eta()[j] * N;
				Real N_j = GET_RND(id_rnd).normal.next();
				m_offspring[i + m_inds.size()]->eta()[j] = m_inds[i]->eta()[j] * exp(m_tau_prime * N + m_tau * N_j);
			}
		}
	}

	template<typename TInd>
	void PopEP<TInd>::select(int id_rnd, int id_pro) {
		std::vector<size_t> wins(m_offspring.size(), 0);
		std::vector<size_t> rand_seq(m_offspring.size());
		std::iota(rand_seq.begin(), rand_seq.end(), 0);
		for (size_t i = 0; i < m_offspring.size(); ++i) {
			GET_RND(id_rnd).uniform.shuffle(rand_seq.begin(), rand_seq.end());
			for (size_t idx = 0; idx < m_q; idx++)
				if (!m_offspring[rand_seq[idx]]->dominate(*m_offspring[i], id_pro))
					wins[i]++;
		}
		std::vector<int> index;
		mergeSort(wins, wins.size(), index, false);
		for (size_t i = 0; i < m_inds.size(); ++i)
			*m_inds[i] = *m_offspring[index[i]];
	}

	template<typename TInd>
	void PopEP<TInd>::resizeOffspring(int id_pro) {
		if (m_offspring.size() > 2 * m_inds.size())
			m_offspring.resize(2 * m_inds.size());
		else if (m_offspring.size() < 2 * m_inds.size()) {
			size_t num_vars = GET_CONOP(id_pro).numVariables(); 
			size_t num_objs = GET_CONOP(id_pro).numObjectives();
			size_t num_cons = GET_CONOP(id_pro).numConstraints();
			for (size_t i = m_offspring.size(); i < 2 * m_inds.size(); ++i)
				m_offspring.emplace_back(new TInd(num_objs, num_cons, num_vars));
		}
	}

	template<typename TInd>
	int PopEP<TInd>::evolve(int id_pro, int id_alg, int id_rnd) {
		if (m_offspring.size() != 2 * m_inds.size())
			resizeOffspring(id_pro);
		mutate(id_rnd, id_pro);
		for (size_t i = m_inds.size(); i < m_offspring.size(); ++i) {
			int tag = m_offspring[i]->evaluate(id_pro, id_alg);
			if (tag == kTerminate)
				return kTerminate;
			if (tag == kInfeasible && GET_CONOP(id_pro).boundaryViolated(*m_offspring[i]))
				GET_CONOP(id_pro).validateSolution(*m_offspring[i], Validation::kSetToBound, id_rnd);
		}
		select(id_rnd, id_pro);
		++this->m_iter;
		return kNormalEval;
	}
}



#endif // !OFEC_EP_POP_H

