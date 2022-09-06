//Register EMO_MMO "EMO-MMO" ConOP,SOP,GOP,MMOP

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
* Ran Cheng, Miqing Li, Ke Li, and Xin Yao
* "Evolutionary Multiobjective Optimization-Based Multimodal Optimization: Fitness Landscape Approximation and Peak Detection."
* IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL. 22, NO. 5, OCTOBER 2018
*-------------------------------------------------------------------------------
* Implemented by Junchen Wang (wangjunchen.chris@gmail.com) at 2020/9/26
*********************************************************************************/

#ifndef OFEC_EMO_MMO_H
#define OFEC_EMO_MMO_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/individual.h"
#include "../../../../core/algorithm/multi_population.h"
#include "../../template/classic/ga/sbx_pop.h"
#include "../../template/classic/de/de_pop.h"
#include <list>
#include <set>
#include <unordered_set>

namespace ofec {
	class transfered_MOP {
	public:
		transfered_MOP(size_t t_max, size_t size_var, size_t size_pop);
		void update_norm_x(const Population<Individual<>> &offspring);
		void update_delta(int t);
		void evaluate_2nd_obj(Population<Individual<>> &offspring);
	private:
		void update_manh_dis();
		int min_manh_dis(size_t i);
		size_t m_t_max, m_N, m_size_var;
		Real m_delta;
		std::vector<std::vector<int>> m_norm_x, m_manh_dis;
	};

	class EMO_MMO : public Algorithm {
	protected:
		void initialize_() override;
		void run_() override;
#ifdef OFEC_DEMO
		void updateBuffer();
	private:
		enum class state { MOFLA, APD, LS };
		state m_state;
#endif

	public:
		void record() override;

	private:
		void MOFLA();
		void binary_cutting_APD();
		void local_search();
		void landscape_cutting(std::unordered_set<size_t> &D_c, Real eta);
		void APD(std::vector<std::vector<Real>> &manh_dis_mat, std::unordered_set<size_t> D_c);
		void update_manh_dis_mat(std::vector<std::vector<Real>> &manh_dis_mat, const std::unordered_set<size_t> &D_c);
		void normc_manh_dis_mat(std::vector<std::vector<Real>> &manh_dis_mat, const std::unordered_set<size_t> &D_c);
		Real min_manh_dis(const std::vector<std::vector<Real>> &manh_dis_mat, const std::unordered_set<size_t> &D_c, size_t i);
		size_t m_size_pop, m_t_max;
		Real m_ratio_MOFLA_FEs;                         // percentage of FEs allocated to fitness landscape approximation
		Real m_eta;                                     // initial cutting ratio
		std::unique_ptr<PopSBX<>> m_MOFLA_pop;                          // population for MOFLA
		MultiPopulation<PopDE<>> m_LS_pops;   // multipopulation for local search
		std::vector<std::unique_ptr<Solution<>>> m_D;   // external archive for approximating fitness landscape
		std::set<std::vector<Real>> m_D_x;              // decision variables of the external archive for uniqueness
		std::vector<std::list<size_t>> m_P;             // detected peak set
	};
}

#endif // !OFEC_EMO_MMO_H

