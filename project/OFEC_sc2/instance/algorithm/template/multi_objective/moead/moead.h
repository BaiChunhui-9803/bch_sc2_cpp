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
// Created: 13 Jan 2015
// Last modified: 25 Aug 2019 by Xiaofang Wu (email:wuxiaofang@cug.edu.cn)

#ifndef OFEC_MOEAD_H
#define OFEC_MOEAD_H

#include<algorithm>
#include<iomanip>
#include<fstream>
#include "../../../multi_objective/moea_de_pop.h"
#include "../../../../../utility/linear_algebra/matrix.h"
#include "../../../../../utility/random/newran.h"

namespace ofec {
	template<typename TInd>
	class MOEAD {
		enum class DecomFun { _TCHE1, _TCHE2, _NBI1, _NBI2, _NBI3 };
	public:
		MOEAD(int id_pro);
		MOEAD(size_t num_objs);

	protected:
		void initialize(std::vector<std::unique_ptr<TInd>>& parent, int id_pro);
		void initUniformWeight(int parent_size, int id_pro);
		void initNeighbourhood();
		void updateReference(TInd& sol, int id_pro);
		void updateProblem(std::vector<std::unique_ptr<TInd>>& parent, TInd& sol, int id, int type, int id_pro, int id_rnd);
		Real fitnessFunction(std::vector<Real>& obj, int k, int id_pro);
		void matingSelection(std::vector<int>& list, int cid, int size, int type, int parent_size, int id_rnd);
		std::vector<Real> mv_ideal_point;	//the best value in every dimension
		std::vector<TInd> mv_sol_arr;	//corresponding TInd of the best value in every dimension
		std::vector<Vector> mv_namda;
		std::vector<std::vector<int> > mvv_neigh;
		int m_unit;
		int m_limit;
		int m_niche;		//number of neighbours
		Real m_realb;     // probability of selecting mating parents from neighborhood
		DecomFun m_decom_function;
	private:
		void minMaxSort(std::vector<Real>& v, std::vector<int>& idx);
	};

	template<typename TInd>
	MOEAD<TInd>::MOEAD(int id_pro) :
		m_unit(13),
		m_niche(20),
		m_realb(0.9),
		m_limit(2),
		m_decom_function(DecomFun::_TCHE1),
		mv_ideal_point(GET_PRO(id_pro).numObjectives(), 1.0e+30) {}

	template<typename TInd>
	MOEAD<TInd>::MOEAD(size_t num_objs) :
		m_unit(13),
		m_niche(20),
		m_realb(0.9),
		m_limit(2),
		m_decom_function(DecomFun::_TCHE1),
		mv_ideal_point(num_objs, 1.0e+30) {}

	template<typename TInd>
	void MOEAD<TInd>::initialize(std::vector<std::unique_ptr<TInd>>& parent, int id_pro) {
		initUniformWeight(parent.size(), id_pro);
		initNeighbourhood();
		for (int i = 0; i < parent.size(); i++)
			updateReference(*(parent[i]), id_pro);
	}

	template<typename TInd>
	void MOEAD<TInd>::initUniformWeight(int parent_size, int id_pro) {
		if (GET_PRO(id_pro).numObjectives() == 2) {
			mv_namda.resize(parent_size);
			for (int n = 0; n < parent_size; n++) {
				Real a = 1.0 * n / (parent_size - 1.);
				mv_namda[n].pushBack(a);
				mv_namda[n].pushBack(1 - a);
			}
		}
		else {
			int n = 0;
			for (int i = 0; i <= m_unit; i++) {
				for (int j = 0; j <= m_unit; j++) {
					if (i + j <= m_unit) {
						std::vector<int> arr;
						arr.push_back(i);
						arr.push_back(j);
						arr.push_back(m_unit - i - j);
						mv_namda.push_back(std::vector<Real>(0));
						for (int k = 0; k < arr.size(); k++)
							mv_namda[n].pushBack(1.0 * arr[k] / m_unit);
						n++;
						arr.clear();
					}
				}
			}
		}
	}

	template<typename TInd>
	void MOEAD<TInd>::initNeighbourhood() {
		int pops = mv_namda.size();
		mvv_neigh.resize(pops);
		std::vector<Real> dis(pops);
		std::vector<int> index(pops);
		for (int i = 0; i < pops; i++) {
			// calculate the distances based on weight vectors
			for (int j = 0; j < pops; j++) {
				dis[j] = mv_namda[i].distance(mv_namda[j]);
				index[j] = j;
			}
			//find 'niche' nearest neighboring subproblems			
			minMaxSort(dis, index);
			for (int k = 0; k < m_niche; k++)
				mvv_neigh[i].push_back(index[k]);
		}
		dis.clear();
		index.clear();
	}

	template<typename TInd>
	void MOEAD<TInd>::updateReference(TInd& sol, int id_pro) {
		//sol: child TInd
		int numObj = GET_PRO(id_pro).numObjectives();
		for (int n = 0; n < numObj; n++) {
			if (sol.objective()[n] < mv_ideal_point[n]) {
				mv_ideal_point[n] = sol.objective()[n];
				mv_sol_arr.emplace_back(sol);
			}
		}
	}

	template<typename TInd>
	void MOEAD<TInd>::updateProblem(std::vector<std::unique_ptr<TInd>>& parent, TInd& sol, int id, int type, int id_pro, int id_rnd) {
		// sol: child TInd
		// id:  the id of current subproblem
		// type:update Individuals in - neighborhood (1) or whole population (otherwise)
		int size, time = 0;
		if (type == 1)
			size = m_niche;
		else
			size = parent.size();
		std::vector<int> perm(size);
		for (int i(0); i < perm.size(); ++i) {
			perm[i] = i;
		}
		GET_RND(id_rnd).uniform.shuffle(perm.begin(), perm.end());

		for (int i = 0; i < size; i++) {
			int k;
			if (type == 1)
				k = mvv_neigh[id][perm[i]];
			else
				k = perm[i];
			// calculate the values of objective function regarding the current subproblem
			Real f1, f2;
			f1 = fitnessFunction(parent[k]->objective(), k, id_pro);
			f2 = fitnessFunction(sol.objective(), k, id_pro);
			if (f2 < f1) {
				*parent[k] = sol;
				time++;
			}
			// the maximal number of TInd updated is not allowed to exceed 'limit'
			if (time >= m_limit)
				return;
		}
		perm.clear();
	}

	template<typename TInd>
	Real MOEAD<TInd>::fitnessFunction(std::vector<Real>& obj, int k, int id_pro) {
		// Chebycheff Scalarizing Function
		Real fitness = 0;
		int numObj = GET_PRO(id_pro).numObjectives();
		//int numObj = obj.size();
		if (m_decom_function == DecomFun::_TCHE1) {
			Real max_fun = -1.0e+30;
			for (int n = 0; n < numObj; n++) {
				//Real diff = fabs(y_obj[n] - idealpoint[n] + scale[n]);
				//Real diff = fabs(y_obj[n] - idealpoint[n] + 0.05);
				Real diff = fabs(obj[n] - mv_ideal_point[n]);
				//Real diff = fabs(y_obj[n] - 0);
				Real feval;
				if (mv_namda[k][n] == 0)
					feval = 0.0001 * diff;
				else
					feval = diff * mv_namda[k][n];
				if (feval > max_fun) max_fun = feval;

			}
			fitness = max_fun;
		}

		if (m_decom_function == DecomFun::_TCHE2) {
			// reference point in the CHIM
			std::vector<int> scale(numObj);
			//throw myException("Please initialize the scale @MOEAD<Poppulation,TInd>::fitnessfuction");
			Real max_fun = -1.0e+30;
			for (int n = 0; n < numObj; n++) {
				Real diff = (obj[n] - mv_ideal_point[n]) / scale[n];  //note: the scale is not initialized, there has no knowledge
				Real feval;
				if (mv_namda[k][n] == 0)
					feval = 0.0001 * diff;
				else
					feval = diff * mv_namda[k][n];
				if (feval > max_fun) max_fun = feval;

			}
			fitness = max_fun;
		}

		// CHIM + Tchebycheff
		// CHIM is not available in 3 objectives
		if (m_decom_function == DecomFun::_NBI1) {
			// quasi normal direction
			Vector norm;
			for (int i = 0; i < numObj; i++) {
				norm.pushBack(0.0);
				for (int j = 0; j < numObj; j++) {
					norm[i] += -mv_sol_arr[j].objective()[i];
				}
			}

			// normalization
			norm.normalize();

			// reference point in the CHIM
			std::vector <Real> base;
			for (int i = 0; i < numObj; i++) {
				Real tp2 = 0;
				for (int j = 0; j < numObj; j++)
					tp2 += mv_sol_arr[j].objective()[i] * mv_namda[k][j];
				base.push_back(tp2);
			}

			// Tchebycheff function
			Real max_fun = -1.0e+30;
			for (int n = 0; n < numObj; n++) {
				Real diff = obj[n] - base[n];
				Real feval = -diff * norm[n];
				if (feval > max_fun) max_fun = feval;

			}
			fitness = max_fun;
		}

		//* Boundary intersection approach
		//* reference point is chosen as the ideal point
		//* the direction is independent of CHIM
		if (m_decom_function == DecomFun::_NBI2) {

			mv_namda[k].normalize();

			// penalty method 
			// temporary vectors NBI method
			Vector realA(numObj);
			Vector realB(numObj);

			// difference beween current point and reference point
			for (int n = 0; n < numObj; n++)
				realA[n] = (obj[n] - mv_ideal_point[n]);

			// distance along the search direction norm
			Real d1 = fabs(realA * mv_namda[k]);

			// distance to the search direction norm
			for (int n = 0; n < numObj; n++)
				realB[n] = (obj[n] - (mv_ideal_point[n] + d1 * mv_namda[k][n]));
			Real d2 = realB.length();

			fitness = (d1 + 5 * d2);

			//t2 = clock();
			//total_sec+=(t2 - t1);
		}

		// NBI method
		if (m_decom_function == DecomFun::_NBI3) {

			// quasi normal direction
			Vector norm;
			for (int i = 0; i < numObj; i++) {
				norm.pushBack(0.0);
				for (int j = 0; j < numObj; j++) {
					norm[i] += -mv_sol_arr[j].objective()[i];
				}
			}

			// normalization
			norm.normalize();

			// reference point in the CHIM
			std::vector<Real> base;
			for (int i = 0; i < numObj; i++) {
				Real tp2 = 0;
				for (int j = 0; j < numObj; j++)
					tp2 += mv_sol_arr[j].objective()[i] * mv_namda[k][j];
				base.push_back(tp2);
			}

			// penalty method 
			// temporary vectors NBI method
			Vector realA;
			Vector realB;

			// difference beween current point and reference point
			for (int n = 0; n < numObj; n++)
				realA.pushBack(obj[n] - base[n]);

			// distance along the search direction norm
			Real d1 = realA * norm;

			// distance to the search direction norm
			for (int n = 0; n < numObj; n++)
				realB.pushBack(obj[n] - (base[n] + d1 * norm[n]));
			Real d2 = realB.length();

			fitness = -d1 + 2 * d2;
		}
		return fitness;
	}

	template<typename TInd>
	void MOEAD<TInd>::matingSelection(std::vector<int>& list, int cid, int size, int type, int parent_size, int id_rnd) {
		// list : the set of the indexes of selected mating parents
		// cid  : the id of current subproblem
		// size : the number of selected mating parents
		// type : 1 - neighborhood; otherwise - whole population
		int r, p;
		while (list.size() < size) {
			if (type == 1) {
				r = GET_RND(id_rnd).uniform.nextNonStd(0, m_niche);
				p = mvv_neigh[cid][r];
			}
			else {
				p = GET_RND(id_rnd).uniform.nextNonStd(0, parent_size);
			}

			bool flag = true;
			for (int i = 0; i < list.size(); i++) {
				if (list[i] == p) { // p is in the list				
					flag = false;
					break;
				}
			}

			if (flag) list.push_back(p);
		}
	}

	template<typename TInd>
	void MOEAD<TInd>::minMaxSort(std::vector<Real>& v, std::vector<int>& idx) {
		for (int i = 0; i < v.size(); ++i) {
			int min = i;
			for (int j = i + 1; j < v.size(); ++j) {
				if (v[j] < v[min])
					min = j;
			}
			auto temp = v[i];
			v[i] = v[min];
			v[min] = temp;
			auto t = idx[i];
			idx[i] = idx[min];
			idx[min] = t;
		}
	}
}

#endif