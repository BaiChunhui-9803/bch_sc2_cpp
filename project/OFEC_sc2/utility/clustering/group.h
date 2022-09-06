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
* a cluser of individuals
*********************************************************************************/
#ifndef OFEC_GROUP_H
#define OFEC_GROUP_H

#include<vector>
#include<memory>
#include"../functional.h"
#include <map>
#include"../../core/definition.h"		//

namespace ofec {
	template <typename Individual>
	class Group {
		std::map<int, Individual*> m_member;
		using itertype = decltype(m_member.begin());
		int m_id = 0, m_best = 0;
		Real m_intra_dis = 0;			//average distance of intra

	public:
		Group() = default;
		void initialize(const std::pair<Individual*, int>  &item) {
			m_member.emplace(item.second, item.first);
			m_best = item.second;
		}
		void merge(Group &g, std::vector<std::vector<Real>> &dis, int id_pro) {
			for (auto& item : g.m_member) {	//update member
				m_member.emplace(item);
			}

			updateBest(g, id_pro);					//update best

			updateDistance(dis);			//update average distance of intra
		}

		int id() {						//id of best individual in group
			return m_id;				
		}

		int best() {					//id of best individual in population
			return m_best;
		}

		int size()const {
			return m_member.size();
		}

		Real intraDis() {
			return m_intra_dis;
		}

		itertype begin() { return m_member.begin(); }

		itertype end() { return m_member.end(); }
		//std::pair<Individual*, int>& operator[](int i) {
		//	return m_member[i];
		//}
		//const std::pair<Individual*, int>& operator[](int i)const {
		//	return m_member.at(i);
		//}

		void updateBest(Group &g, int id_pro) {	//update best
			
			Dominance r = objectiveCompare(m_member[m_best]->objective(), 
				g.m_member[g.m_best]->objective(), GET_PRO(id_pro).optMode());

			if (Dominance::kDominated == r) {
				m_best = g.m_best;
				m_id = m_member.size() - g.size() + g.id();
			}
				
			else if (Dominance::kNonDominated==r){
				int rank[2] = {0,0};
				
				if (GET_PRO(id_pro).optMode(0) == OptMode::kMinimize) {
					for (int i = 0; i < m_member[0]->objectiveSize(); ++i) {
						if (m_member[m_best]->objective(i) < m_member[g.m_best]->objective(i))
							rank[0]++;
						else if (m_member[m_best]->objective(i) > m_member[g.m_best]->objective(i))
							rank[1]++;
					}
				}
				else {
					for (int i = 0; i < m_member[0]->objectiveSize(); ++i) {
						if (m_member[m_best]->objective(i) > m_member[g.m_best]->objective(i))
							rank[0]++;
						else if (m_member[m_best]->objective(i) < m_member[g.m_best]->objective(i))
							rank[1]++;
					}
				}
				
				if (rank[0] < rank[1]) {
					m_best = g.m_best;
					m_id = m_member.size() - g.size() + g.id();
				}				
			}
		}

		void updateDistance(const std::vector<std::vector<Real>> &dis) {				//update average distance of intra
			Real sum = 0;
			for (auto iter1 = m_member.begin(); iter1 != m_member.end(); ++iter1) {
				for (auto iter2 = iter1; iter2 != m_member.end(); ++iter2) {
					sum += dis[iter1->first][iter2->first];
				}
			}
			int c = m_member.size()*(m_member.size() - 1) / 2;
			m_intra_dis = sum / c;
		}
	};
}
#endif // GROUP_H
