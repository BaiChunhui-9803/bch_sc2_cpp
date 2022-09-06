//Register MultiDimensionalKnapsack "MKP" MKP,ComOP,SOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 18 Apr 2016
//ע�� �����ⱻת��Ϊ��С������, �������ý��û�о�ȷ����Ƿ��ǺϷ���
// Modified: 14 Mar 2018 By Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef MKP_H
#define MKP_H

#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/domain.h"

namespace ofec {
#define GET_MKP(id_pro) dynamic_cast<MultiDimensionalKnapsack&>(GET_PRO(id_pro))

	class MultiDimensionalKnapsack : public Problem {
	protected:
		size_t m_num_vars;
		std::vector<Real> mv_p;
		std::vector<std::vector<Real>> mvv_r;
		std::vector<Real> mv_b;
		std::string m_file_name;
		Optima<VarVec<int>> m_optima;
		int m_m;
		Real m_maxP;
		bool m_if_valid_check = true;

	public:
		void initSolution(SolBase &s, int id_rnd) const override;
		bool same(const SolBase &s1, const SolBase &s2) const override;
		Real variableDistance(const SolBase &s1, const SolBase &s2) const override;
		Real variableDistance(const VarBase &x1, const VarBase &x2) const override;
		bool isOptimaObjGiven() const override { return m_optima.isObjectiveGiven(); }
		bool isOptimaVarGiven() const override { return m_optima.isVariableGiven(); }

		bool isValid(const SolBase &s) const;
		Real getConstraintValue(const SolBase &s, std::vector<Real> &val) const;
		int numInvalidConstraints(SolBase &s) const;
		const Optima<VarVec<int>>& get_optima() const { return m_optima; }
		Optima<VarVec<int>>& get_optima() { return m_optima; }
		
	protected:
		void initialize_();
		void evaluate_(SolBase &s, bool effective) override;
		void read_problem();
	};
}

#endif // !MKP_H

