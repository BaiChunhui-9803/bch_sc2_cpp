/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia & Junchen Wang
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 7 Jan 2015
// Last modified: 15 Aug 2019 by Junchen Wang (email:wangjunchen.chris@gmail.com)

/*-----------------------------------------------------------------------------------
   See the details of NSGA2 in the following paper
   A Fast and Elitist Multiobjective Genetic Algorithm : NSGA - II
   Kalyanmoy Deb, Associate Member, IEEE, Amrit Pratap, Sameer Agarwal, and T.Meyarivan
   IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL. 6, NO. 2, APRIL 2002
-----------------------------------------------------------------------------------*/

#ifndef OFEC_NSGAII_H
#define OFEC_NSGAII_H

#include "../../../../../utility/nondominated_sorting/fast_sort.h"

namespace ofec {
	class NSGAII {
	protected:
		size_t m_num_obj;
		std::vector<OptMode> m_opt_mode;

	public:
		NSGAII(size_t num_obj, const std::vector<OptMode> &opt_mode): 
			m_num_obj(num_obj),
			m_opt_mode(opt_mode) {}

		template<typename TPop, typename TPopCmbnd>
		void survivorSelection(TPop &pop, TPopCmbnd &pop_combined) {
			nondominatedSorting(pop_combined);
			evalDens(pop, pop_combined);
		}

	protected:
		template<typename TPopCmbnd>
		void nondominatedSorting(TPopCmbnd &pop_combined) {
			std::vector<std::vector<Real> *> objs;
			for (size_t i = 0; i < pop_combined.size(); ++i)
				objs.emplace_back(&pop_combined[i].objective());
			std::vector<int> rank;
			nd_sort::fastSort<Real>(objs, rank, m_opt_mode);
			for (size_t i = 0; i < pop_combined.size(); ++i)
				pop_combined[i].setRank(rank[i]);
		}

		template<typename TPop, typename TPopCmbnd>
		void evalDens(TPop &pop, TPopCmbnd &pop_combined) {
			int pops = 0;  //indicate parent population size be 0
			int size = pop_combined.size();
			int rank = 0;
			while (true) {
				int count = 0;
				for (size_t i = 0; i < size; i++)
					if (pop_combined[i].rank() == rank)
						count++;
				int size2 = pops + count;
				if (size2 > pop.size()) {
					break;
				}
				for (size_t i = 0; i < size; i++)
					if (pop_combined[i].rank() == rank)
					{
						pop[pops] = pop_combined[i];
						++pops;
					}
				rank++;
				if (pops >= pop.size()) break;
			}
			if (pops < pop.size()) {
				std::vector<int> list;
				// save the individuals in the overflowed front
				for (size_t i = 0; i < size; i++)
					if (pop_combined[i].rank() == rank)
						list.push_back(i);
				int s2 = list.size();
				std::vector<Real> density(s2);
				std::vector<Real> obj(s2);
				std::vector<int> idx(s2);
				std::vector<int> idd(s2);
				for (size_t i = 0; i < s2; i++) {
					idx[i] = i;
					density[i] = 0;
				}
				for (size_t j = 0; j < m_num_obj; j++) {
					for (size_t i = 0; i < s2; i++) {
						idd[i] = i;
						obj[i] = pop_combined[list[i]].objective()[j];
					}
					mergeSort(obj, s2, idd, true, 0, s2 - 1, s2);
					density[idd[0]] += -1.0e+30;
					density[idd[s2 - 1]] += -1.0e+30;
					for (int k = 1; k < s2 - 1; k++)
						density[idd[k]] += -(obj[idd[k]] - obj[idd[k - 1]] + obj[idd[k + 1]] - obj[idd[k]]);
				}
				idd.clear();
				obj.clear();
				int s3 = pop.size() - pops;
				mergeSort(density, s2, idx, true, 0, s2 - 1, s3);
				for (size_t i = 0; i < s3; i++) {
					pop[pops] = pop_combined[list[idx[i]]];
					++pops;
				}
				density.clear();
				idx.clear();
				list.clear();
			}
		}
	};
}
#endif //!OFEC_NSGAII_H
