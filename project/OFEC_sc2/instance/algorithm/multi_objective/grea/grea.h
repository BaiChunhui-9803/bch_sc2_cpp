//Register GrEA "GrEA" ConOP,MOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Xiaofang Wu
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*  See the details of CDG-MOEA in the following paper
*  S.Yang, M.Li, X.Liu, et al. A Grid-Based Evolutionary Algorithm for Many-Objective 
*  Optimization. IEEE Transactions on Evolutionary Computation, 2013, 17(5):721-736.
*************************************************************************/
// Created: 4 May 2019
// Last modified: 23 Aug 2019 by Xiaofang Wu(email:email:wuxiaofang@cug.edu.cn)

#ifndef OFEC_GREA_H
#define OFEC_GREA_H

#include<iostream>
#include<vector>
#include<memory>
#include "../../../../core/algorithm/individual.h"
#include "../../../../core/algorithm/population.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../template/classic/ga/sbx_pop.h"
#include "../../template/multi_objective/nsgaii/nsgaii.h"

namespace ofec {
	/***************************individual******************************/
	class IndGrEA : public Individual<> {
		std::vector<int> m_Gk;
		double m_GR;
		double m_GCD = 0;
		double m_GCPD;			
	public:
		IndGrEA(size_t num_objs, size_t num_cons, size_t num_vars) :
			Individual<>(num_objs, num_cons, num_vars),
			m_Gk(num_objs) {}
		int& Gk(int idx) { return m_Gk[idx]; }
		const double& GR() const { return m_GR; }
		double& GR() { return m_GR; }
		const double& GCD() const { return m_GCD; }
		double& GCD() { return m_GCD; }
		const double& GCPD() const { return m_GCPD; }
		double& GCPD() { return m_GCPD; }			
	};

	/******************************population*************************/		
	class PopGrEA : public PopSBX<IndGrEA>, public NSGAII {
	public:
		PopGrEA(size_t size_pop,int id_pro,size_t size_obj);
		void initialize(int id_pro, int id_rnd);
		int evolve(int id_pro,int id_alg,int id_rnd) override;
		int evolveMO(int id_pro, int id_alg, int id_rnd);
		void evalEens(int id_pro, int id_rnd);
		void gridConstruct(int id_pro);	// construct grid environment
		void gridConstructFi(Population<IndGrEA> &offspring,std::vector<int>& Fi, int size,int id_pro);
		void assignGR_GCPD(int id_pro);	// assign GR and GCPD for individuals in the population
		void assignGR_GCPD_Fi(Population<IndGrEA> &offspring,std::vector<int>& Fi, int size,int id_pro);
		void assignGCD(int id_pro);		// assign GCD for individuals in the population
		int checkDominanceGrid(IndGrEA &a, IndGrEA &b,int id_pro);
		
	protected:
		std::vector<std::pair<Real, Real>> m_ind_min_max; //first is m_ind_min,and second is m_ind_max
		std::vector<std::pair<Real, Real>> m_grid_min_max;
		std::vector<Real> m_grid_distance;
		int m_grid_div = 8;
		Population<IndGrEA> m_offspring;  // 2 size of population			
	};

	/******************************GrEA*******************************/
	class GrEA :public Algorithm {
	public:
		void initialize_() override;
		void record() override;
		void initPop();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif
	protected:
		void run_() override;
	protected:
		std::unique_ptr<PopGrEA> m_pop;
		size_t m_pop_size;
	};
}
#endif