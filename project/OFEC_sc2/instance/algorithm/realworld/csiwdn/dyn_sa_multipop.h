/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Jan 19, 2019 by Li Zhou

#ifndef OFEC_DYNSAMULTIPOP
#define OFEC_DYNSAMULTIPOP

#include "../../../../core/algorithm/multi_population.h"
#include "../../../problem/realworld/csiwdn/csiwdn.h"

namespace ofec {

	template <typename TPop>
	class DynSaMultiPop : public MultiPopulation<TPop>
	{
		using PAIR = std::pair<VarCSIWDN, Real>;
		struct CmpByValue {
			bool operator()(const PAIR& lhs, const PAIR& rhs) {
				return lhs.second < rhs.second;
			}
		};
	public:
		DynSaMultiPop() = default;
		DynSaMultiPop(size_t n, size_t subsize,int id_pro): MultiPopulation<TPop>(n,subsize, id_pro){}
		void populationCompetition(size_t indx1, size_t indx2);
		bool isHighCoverageRate(int id_pro, size_t indx1, size_t indx2, double maxrate);
		bool isStable(int id_pro);
		void addNewPopulation(int id_alg, int id_pro, int id_rnd, size_t num, InitType type);
	protected:
		std::vector<size_t> m_count_last;
	};

	template <typename TPop>
	void DynSaMultiPop<TPop>::populationCompetition(size_t indx1, size_t indx2) {
		
		size_t sub_size1 = MultiPopulation<TPop>::m_pops[0]->m_first_pop->size();
		std::vector<std::pair<VarCSIWDN, Real>> temp1;
		for (size_t i = 0; i < sub_size1; ++i)
			temp1.push_back(std::make_pair((*(MultiPopulation<TPop>::m_pops[indx1]->m_first_pop))[i].variable(), (*(MultiPopulation<TPop>::m_pops[indx1]->m_first_pop))[i].objective(0)));
		for (size_t i = 0; i < sub_size1; ++i)
			temp1.push_back(std::make_pair((*(MultiPopulation<TPop>::m_pops[indx2]->m_first_pop))[i].variable(), (*(MultiPopulation<TPop>::m_pops[indx2]->m_first_pop))[i].objective(0)));
		sort(temp1.begin(), temp1.end(), CmpByValue());
		for (size_t i = 0; i < sub_size1; ++i) {
			(*(MultiPopulation<TPop>::m_pops[indx1]->m_first_pop))[i].variable() = temp1[i].first;
			(*(MultiPopulation<TPop>::m_pops[indx1]->m_first_pop))[i].objective()[0] = temp1[i].second;
		}

		size_t sub_size2 = MultiPopulation<TPop>::m_pops[0]->m_second_pop->size();
		std::vector<std::pair<VarCSIWDN, Real>> temp2;
		for (size_t i = 0; i < sub_size2; ++i)
			temp2.push_back(std::make_pair((*(MultiPopulation<TPop>::m_pops[indx1]->m_second_pop))[i].variable(), (*(MultiPopulation<TPop>::m_pops[indx1]->m_second_pop))[i].objective(0)));
		for (size_t i = 0; i < sub_size2; ++i)
			temp2.push_back(std::make_pair((*(MultiPopulation<TPop>::m_pops[indx2]->m_second_pop))[i].variable(), (*(MultiPopulation<TPop>::m_pops[indx2]->m_second_pop))[i].objective(0)));
		sort(temp2.begin(), temp2.end(), CmpByValue());
		for (size_t i = 0; i < sub_size2; ++i) {
			(*(MultiPopulation<TPop>::m_pops[indx1]->m_second_pop))[i].variable() = temp2[i].first;
			(*(MultiPopulation<TPop>::m_pops[indx1]->m_second_pop))[i].objective()[0] = temp2[i].second;
		}

	}

	template <typename TPop>
	bool DynSaMultiPop<TPop>::isHighCoverageRate(int id_pro,size_t indx1, size_t indx2, double maxrate) {
		int size1 = MultiPopulation<TPop>::m_pops[indx1]->m_first_pop->size();
		int size2 = MultiPopulation<TPop>::m_pops[indx2]->m_first_pop->size();
		std::vector<bool> flag(size2, true);
		int count = 0;
		for (size_t i = 0; i < size1; ++i) {
			for (size_t j = 0; j < size2; ++j) {
				int z = 0;
				while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= (*(MultiPopulation<TPop>::m_pops[indx1]->m_first_pop))[i].variable().startTime(z + 1)) {
					z++;
				}
				if (flag[j] && (*(MultiPopulation<TPop>::m_pops[indx1]->m_first_pop))[i].variable().index(z) == (*(MultiPopulation<TPop>::m_pops[indx2]->m_first_pop))[j].variable().index(z)) {
					++count;
					flag[j] = false;
					break;
				}
			}
		}
		double rate = (double)count / (double)size1;
		return rate > maxrate;
	}

	template <typename TPop>
	bool DynSaMultiPop<TPop>::isStable(int id_pro) {
		size_t node_size = GET_CSIWDN(id_pro).numberNode();
		std::vector<size_t> count(node_size, 0);
		int num_pop = MultiPopulation<TPop>::size();
		for (size_t i = 0; i < num_pop; ++i) {
			std::vector<bool> flag(node_size, false);
			for (size_t j = 0; j < MultiPopulation<TPop>::m_pops[i]->m_first_pop->size(); ++j) {
				int z = 0;
				while ((z + 1) < GET_CSIWDN(id_pro).numSource() && GET_CSIWDN(id_pro).phase() >= (*(MultiPopulation<TPop>::m_pops[i]->m_first_pop))[j].variable().startTime(z + 1)) {
					z++;
				}
				size_t temp = (*(MultiPopulation<TPop>::m_pops[i]->m_first_pop))[j].variable().index(z) - 1;
				if (!flag[temp]) {
					++count[temp];
					flag[temp] = true;
				}
			}
		}
		
		if (m_count_last.size() == 0) {
			m_count_last = count;
			return false;
		}
		for (size_t i = 0; i < count.size(); ++i) {
			if (m_count_last[i] != count[i]) {
				m_count_last = count;
				return false;
			}

		}
		return true;


	
	}

	template <typename TPop>
	void DynSaMultiPop<TPop>::addNewPopulation(int id_alg,int id_pro,int id_rnd,size_t num, InitType type) {
		size_t popsize = MultiPopulation<TPop>::m_pops[0]->m_first_pop->size();
		size_t flag = MultiPopulation<TPop>::size();
		for (size_t i = 0; i < num; ++i)
			MultiPopulation<TPop>::m_pops.emplace_back(std::move(std::unique_ptr<TPop>(new TPop(popsize,id_pro))));
		
		if (type == InitType::Random)
			GET_CSIWDN(id_pro).setInitType(InitType::Random);
		else if (type == InitType::BeVisited) {
			GET_CSIWDN(id_pro).setInitType(InitType::BeVisited);
			GET_CSIWDN(id_pro).calProByBeVisited();
		}
		else if (type == InitType::Distance)
			GET_CSIWDN(id_pro).setInitType(InitType::Distance);
		else throw MyExcept("No this initialization type");

		for (size_t i = 0; i < flag; i++) {
			this->m_pops[i]->updateBest(id_pro,id_alg);
			this->m_pops[i]->m_best;
		}

		for (size_t i = flag; i < flag + num; ++i) {
			GET_CSIWDN(id_pro).setPopIdentifier(i - flag);
			size_t idx_pop = GET_RND(id_rnd).uniform.nextNonStd<size_t>(0, flag);
			auto& var_best = this->m_pops[idx_pop]->m_best->variable();
			MultiPopulation<TPop>::m_pops[i]->initializeNewPop(id_alg,id_pro,id_rnd,var_best);
		}
	}


}

#endif