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
* class Population manages a set of individuals.
*
*********************************************************************************/

#ifndef OFEC_POPULATION_H
#define OFEC_POPULATION_H

#include "../global.h"
#include "../../utility/functional.h"
#include "../../utility/nondominated_sorting/fast_sort.h"
#include "../../utility/nondominated_sorting/filter_sort.h"
#include "../instance_manager.h"

namespace ofec {
	template<typename>  class MultiPopulation;

	template<typename TInd>
	class Population {
	public:
		virtual ~Population() = default;
		template<typename> friend class MultiPopulation;
		using IndType = TInd;
		using SolType = typename IndType::SolType;
		using IterType = typename std::vector<std::unique_ptr<IndType>>::iterator;
		using CIterType = typename std::vector<std::unique_ptr<IndType>>::const_iterator;
		//element access
		size_t size() const noexcept { return m_inds.size(); }
		const IndType& operator[](size_t i) const { return *m_inds[i]; }
		IndType& operator[](size_t i) { return *m_inds[i]; }
		const IndType &at(size_t i) const { return *m_inds[i]; }
		IndType& at(size_t i) { return *m_inds[i]; }
		IterType begin() { return m_inds.begin(); }
		IterType end() { return m_inds.end(); }
		CIterType begin() const { return m_inds.begin(); }
		CIterType end() const { return m_inds.end(); }
		const std::list<std::unique_ptr<SolType>> &best() const { return m_best; }
		const std::list<std::unique_ptr<SolType>> &worst() const { return m_worst; }
		std::vector<int> order(int idx) const; // get individual(s) with order idx
		size_t& iteration() { return m_iter; }
		size_t iteration() const { return m_iter; }
		int timeBestUpdated() const { return m_time_best_updated; }
		int timeWorstUpdated() const { return m_time_worst_updated; }

		//update	
		const std::list<std::unique_ptr<SolType>>& best(int id_pro) {
			updateBest(id_pro);
			return m_best;
		}
		const std::list<std::unique_ptr<SolType>>& worst(int id_pro) {
			updateWorst(id_pro);
			return m_worst;
		}
		void updateBest(int id_pro);
		void updateWorst(int id_pro);

		void handleEvalTag(int tag) {}

		//constructors and members
		Population() = default;
		template<typename ... Args>
		Population(size_t n, int id_pro, Args&& ...args);
		Population(const Population &rhs);
		Population &operator=(const Population &rhs);
		Population(Population &&rhs);
		Population &operator=(Population &&rhs);

		//operations
		virtual void append(const IndType &ind);
		virtual void append(IndType &&ind);
		virtual void append(std::unique_ptr<IndType> &ind);
		virtual void append(Population<IndType> &pop);
		virtual IterType remove(IterType iter_ind) { return m_inds.erase(iter_ind); }
		virtual void sort(int id_pro);

		template<typename ... Args>
		void assign(size_t n, int id_pro, Args&& ...args);

		virtual void initialize(int id_pro, int id_rnd); //a uniformly distributed initialization by default
		template<typename Fun, typename Problem, typename... Args>
		void initialize(Fun fun, const Problem *pro, Args && ... args);
		virtual int evaluate(int id_pro, int id_alg); // evaluate each individual 
		virtual int evolve(int id_pro, int id_alg, int id_rnd) { return 0; }
		void reset(); // delete all individuals
		std::map<Real, size_t> nearestNeighbour(size_t idx, int id_pro, int k = 1);
		void resetImproved();
		Real rank(const SolType &x, int id_pro);
		int numRanks() { return m_order.rbegin()->first; }
		int id() const { return m_id; }
		void setId(int id) { m_id = id; }
		bool isActive()const noexcept { return m_active; }
		void setActive(bool value) noexcept { m_active = value; }
		template<typename ... Args>
		void resize(size_t size, int id_pro, Args&& ...args);
		void clear();
		virtual void updateFitness(int id_pro);// update the fitness of m_inds
		virtual void updateFitness(int id_pro, IndType &ind) const;

	protected:
		bool updateBest(const SolType &sol, int id_pro);
		bool updateWorst(const SolType &sol, int id_pro);

	protected:
		int m_id = -1;
		size_t m_iter = 0;			// the current number of iterations
		bool m_active = true;
		int m_time_best_updated = -1, m_time_worst_updated = -1;
		std::multimap<int, int> m_order;
		std::vector<std::unique_ptr<IndType>> m_inds;
		std::list<std::unique_ptr<SolType>> m_best, m_worst;   // best so far
	};

	template<typename TInd>
	std::vector<int> Population<TInd>::order(int idx) const {
		auto range_ = m_order.equal_range(idx);
		std::vector<int> inds;
		for (auto i = range_.first; i != range_.second; ++i)
			inds.push_back(i->second);
		return inds;
	}

	template<typename TInd>
	void Population<TInd>::updateBest(int id_pro) {
		for (auto &ptr_ind : m_inds) {
			if (updateBest(ptr_ind->phenotype(), id_pro))
				m_time_best_updated = m_iter;
		}
	}

	template<typename TInd>
	void Population<TInd>::updateWorst(int id_pro) {
		for (auto &ptr_ind : m_inds)
			if (updateWorst(ptr_ind->phenotype(), id_pro))
				m_time_worst_updated = m_iter;
	}

	template<typename TInd>
	void Population<TInd>::clear() {
		m_iter = 0;
		m_active = true;
		m_time_best_updated = -1;
		m_time_worst_updated = -1;
		m_order.clear();
		m_inds.clear();
		m_best.clear();
		m_worst.clear();
	}

	template<typename TInd>
	bool Population<TInd>::updateBest(const SolType &x, int id_pro) {
		bool is_best = true;
		for (auto iter = m_best.begin(); iter != m_best.end();) {
			auto result = x.compare(**iter, GET_PRO(id_pro).optMode());
			if (result == Dominance::kDominated || result == Dominance::kEqual) {
				is_best = false;
				break;
			}
			else if (result == Dominance::kDominant)
				iter = m_best.erase(iter);
			else
				iter++;
		}
		if (is_best) {
			m_best.emplace_back(new SolType(x));
			m_time_best_updated = m_iter;
		}
		return is_best;
	}

	template<typename TInd>
	bool Population<TInd>::updateWorst(const SolType &x, int id_pro) {
		bool is_worst = true;
		for (auto iter = m_worst.begin(); iter != m_worst.end();) {
			auto result = x.compare(**iter, GET_PRO(id_pro).optMode());
			if (result == Dominance::kDominant) {
				is_worst = false;
				break;
			}
			else if (result == Dominance::kDominated)
				iter = m_worst.erase(iter);
			else
				iter++;
		}
		if (is_worst)
			m_worst.emplace_back(new SolType(x));
		return is_worst;
	}

	template<typename TInd>
	void Population<TInd>::updateFitness(int id_pro) {
		if (GET_PRO(id_pro).hasTag(ProTag::kMOP)) {
			sort(id_pro);
			for (auto &it : m_inds) {
				it->setFitness(it->rank());
			}
		}
		else {
			if (GET_PRO(id_pro).optMode()[0] == OptMode::kMaximize) {
				for (auto &it : m_inds) {
					it->setFitness(1.0 / it->objective(0));
				}
			}
			else {
				for (auto &it : m_inds) {
					it->setFitness(it->objective(0));
				}
			}
		}
	}

	template<typename TInd>
	void Population<TInd>::updateFitness(int id_pro, IndType &ind) const {
		if (GET_PRO(id_pro).hasTag(ProTag::kMOP)) {
			ind.setFitness(ind.rank());
		}
		else {
			if (GET_PRO(id_pro).optMode()[0] == OptMode::kMaximize) {
				ind.setFitness(1.0 / ind.objective(0));
			}
			else {
				ind.setFitness(ind.objective(0));
			}
		}
	}

	template<typename TInd>
	template<typename ... Args>
	Population<TInd>::Population(size_t n, int id_pro, Args&& ... args) : m_inds(n) {
		size_t num_objs = GET_PRO(id_pro).numObjectives();
		size_t num_cons = GET_PRO(id_pro).numConstraints();
		for (auto &i : m_inds)
			i = std::make_unique<IndType>(num_objs, num_cons, std::forward<Args>(args)...);
	}

	template<typename TInd>
	Population<TInd>::Population(const Population &rhs) :
		m_id(rhs.m_id),
		m_iter(rhs.m_iter),
		m_active(rhs.m_active),
		m_time_best_updated(rhs.m_time_best_updated),
		m_time_worst_updated(rhs.m_time_worst_updated),
		m_order(rhs.m_order)
	{
		for (size_t i = 0; i < rhs.size(); i++)
			m_inds.push_back(std::make_unique<IndType>(rhs[i]));
		for (auto &ptr_sol : rhs.m_best)
			m_best.emplace_back(new SolType(*ptr_sol));
		for (auto &ptr_sol : rhs.m_worst)
			m_worst.emplace_back(new SolType(*ptr_sol));
	}

	template<typename TInd>
	Population<TInd> &Population<TInd>::operator=(const Population &rhs) {
		if (&rhs == this)
			return *this;
		m_id = rhs.m_id;
		m_iter = rhs.m_iter;
		m_active = rhs.m_active;
		m_time_best_updated = rhs.m_time_best_updated;
		m_time_worst_updated = rhs.m_time_worst_updated;
		m_order = rhs.m_order;
		m_inds.clear();
		m_best.clear();
		m_worst.clear();
		for (size_t i = 0; i < rhs.size(); i++)
			m_inds.push_back(std::make_unique<IndType>(rhs[i]));
		for (auto &ptr_sol : rhs.m_best)
			m_best.emplace_back(new SolType(*ptr_sol));
		for (auto &ptr_sol : rhs.m_worst)
			m_worst.emplace_back(new SolType(*ptr_sol));
		return *this;
	}

	template<typename TInd>
	Population<TInd>::Population(Population &&rhs) :
		m_id(rhs.m_id),
		m_iter(rhs.m_iter),
		m_active(rhs.m_active),
		m_time_best_updated(rhs.m_time_best_updated),
		m_time_worst_updated(rhs.m_time_worst_updated),
		m_order(rhs.m_order),
		m_inds(std::move(rhs.m_inds)),
		m_best(std::move(rhs.m_best)),
		m_worst(std::move(rhs.m_worst)) {}

	template<typename TInd>
	Population<TInd> &Population<TInd>::operator=(Population &&rhs) {
		if (&rhs == this)
			return *this;
		m_id = rhs.m_id;
		m_iter = rhs.m_iter;
		m_active = rhs.m_active;
		m_time_best_updated = rhs.m_time_best_updated;
		m_time_worst_updated = rhs.m_time_worst_updated;
		m_order = std::move(rhs.m_order);
		m_inds = std::move(rhs.m_inds);
		m_best = std::move(rhs.m_best);
		m_worst = std::move(rhs.m_worst);
		return *this;
	}

	template<typename TInd>
	void Population<TInd>::reset() {
		m_iter = 0;
		m_active = true;
		m_time_best_updated = -1;
		m_order.clear();
		m_inds.clear();
		m_best.clear();
		m_worst.clear();
	}

	template<typename TInd>
	void Population<TInd>::initialize(int id_pro, int id_rnd) {
		for (int i = 0; i < m_inds.size(); ++i) {
			m_inds[i]->initialize(i, id_pro, id_rnd);
		}
	}

	template<typename TInd>
	template<typename ...Args>
	inline void Population<TInd>::assign(size_t n, int id_pro, Args && ...args)
	{
		m_id = -1;
		m_active = true;
		m_iter = 0;
		m_time_best_updated = -1;
		m_time_worst_updated = -1;
		m_order.clear();
		m_best.clear();
		m_worst.clear();
		m_inds.resize(n);
		size_t num_objs = GET_PRO(id_pro).numObjectives();
		size_t num_cons = GET_PRO(id_pro).numConstraints();
		for (auto &i : m_inds)
			i = std::make_unique<IndType>(num_objs, num_cons, std::forward<Args>(args)...);

	}

	template<typename TInd>
	template<typename Fun, typename Problem, typename... Args>
	void  Population<TInd>::initialize(Fun fun, const Problem *pro, Args&& ... args) {
		fun(m_inds, pro, std::forward<Args>(args)...);
	}

	template<typename TInd>
	void Population<TInd>::sort(int id_pro) {
		std::vector<std::vector<Real> *> data(m_inds.size());
		for (size_t i = 0; i < m_inds.size(); ++i)
			data[i] = &(m_inds[i]->objective());
		std::vector<int> rank;
		//nd_sort::fastSort<Real>(data, rank, global::ms_global->m_problem->optMode());
		if (m_inds.size() > 1e3) {
			nd_sort::filterSortP<Real>(data, rank, GET_PRO(id_pro).optMode());
		}
		else {
			nd_sort::filterSort<Real>(data, rank, GET_PRO(id_pro).optMode());
		}

		m_order.clear();
		for (size_t i = 0; i < m_inds.size(); i++) {
			m_order.insert(std::pair<int, int>(rank[i], i));
			m_inds[i]->setRank(rank[i]);
		}
	}

	template<typename TInd>
	std::map<Real, size_t> Population<TInd>::nearestNeighbour(size_t idx, int id_pro, int k) {
		std::map<Real, size_t> result;
		Real dis;
		for (size_t i = 0; i < m_inds.size(); i++) {
			if (i != idx) {
				dis = m_inds[idx]->variableDistance(*m_inds[i], id_pro);
				result[dis] = i;
			}
		}
		auto iter = result.begin();
		for (size_t i = 0; i < k; i++)
			iter++;
		result.erase(iter, result.end());
		return result;
	}

	template<typename TInd>
	int Population<TInd>::evaluate(int id_pro, int id_alg) {
		int tag = kNormalEval;
		for (auto &i : m_inds) {
			tag = i->evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
		}
		return tag;
	}

	template<typename TInd>
	void Population<TInd>::resetImproved() {
		for (auto &i : m_inds) {
			i->setImproved(false);
		}
	}

	template<typename TInd>
	Real Population<TInd>::rank(const SolType &x, int id_pro) {
		int i = 0;
		while (1) {
			auto l = m_order.lower_bound(i), u = m_order.upper_bound(i);
			while (l != u) {
				Dominance r = objectiveCompare(x.objective(), m_inds[l->second]->objective(), GET_PRO(id_pro).optMode());
				if (r == Dominance::kDominant) return i - 0.5;
				if (r == Dominance::kDominated) break;
				++l;
			}
			if (l == u) return i;
			if (u == m_order.end()) return i + 0.5;
			++i;
		}
	}

	template<typename TInd>
	void Population<TInd>::append(IndType &&ind) {
		m_inds.emplace_back(new IndType(std::forward<IndType>(ind)));
		/* Set id */
		// TODO
	}

	template<typename TInd>
	void Population<TInd>::append(std::unique_ptr<IndType> &ind) {
		m_inds.emplace_back(ind.release());
		/* Set id */
		// TODO
	}

	template<typename TInd>
	void Population<TInd>::append(const IndType &ind) {
		m_inds.emplace_back(new IndType(ind));
		/* Set id */
		// TODO
	}

	template<typename TInd>
	void Population<TInd>::append(Population<IndType> &pop) {
		m_inds.resize(m_inds.size() + pop.m_inds.size());
		for (size_t i = m_inds.size(); i < pop.m_inds.size(); i++) {
			m_inds[i].reset(pop.m_inds[i - m_inds.size()].release());
		}
		/* Set id */
		// TODO
	}

	template<typename TInd>
	template<typename ... Args>
	void Population<TInd>::resize(size_t size, int id_pro, Args&& ... args) {
		if (m_inds.size() > size)
			m_inds.resize(size);
		else if (m_inds.size() < size) {
			size_t num_objs = GET_PRO(id_pro).numObjectives();
			size_t num_cons = GET_PRO(id_pro).numConstraints();
			for (size_t i = m_inds.size(); i < size; i++)
				m_inds.emplace_back(new IndType(num_objs, num_cons, std::forward<Args>(args)...));
		}
		/* Set id */
		// TODO
	}
}

#endif // !OFEC_POPULATION_H
