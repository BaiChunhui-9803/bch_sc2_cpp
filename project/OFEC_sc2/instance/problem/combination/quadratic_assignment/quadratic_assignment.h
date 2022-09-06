//Register QuadraticAssignment "QAP" QAP,ComOP,SOP

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
// Modified: 14 Mar 2018 By Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef QAP_H
#define QAP_H

#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/domain.h"

namespace ofec {
#define GET_QAP(id_pro) dynamic_cast<QuadraticAssignment&>(GET_PRO(id_pro))

	class QuadraticAssignment : public Problem {
	protected:
		size_t m_num_vars;
		std::vector<std::vector<Real> > mvv_flow;
		std::vector<std::vector<Real> > mvv_distance;
		std::string m_file_name;
		Domain<int> m_domain;
		Optima<VarVec<int>> m_optima;
		bool m_if_valid_check = true;
	public:
		void initSolution(SolBase &s, int id_rnd) const override;
		bool same(const SolBase &s1, const SolBase &s2) const override;
		Real variableDistance(const SolBase &s1, const SolBase &s2) const override;
		Real variableDistance(const VarBase &x1, const VarBase &x2) const override;
		bool isOptimaObjGiven() const override { return m_optima.isObjectiveGiven(); }
		bool isOptimaVarGiven() const override { return m_optima.isVariableGiven(); }
		void updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const override;
		size_t numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const override;
		int updateEvalTag(SolBase &s, int id_alg, bool effective_eval) override;

		size_t numVariables() const override { return m_num_vars; }
		const Optima<VarVec<int>> &getOptima() const { return m_optima; }
		Optima<VarVec<int>> &getOptima() { return m_optima; }
		bool isValid(const SolBase &s) const;

	protected:
		void initialize_() override;
		void readProblem();    //read source data from file
		void evaluate_(SolBase& s, bool effective) override;
	};
}

#endif // !QAP_H

