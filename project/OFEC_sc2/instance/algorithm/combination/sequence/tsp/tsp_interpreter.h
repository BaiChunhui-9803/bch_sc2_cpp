/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Yiya Diao
* Email: diaoyiyacug@163.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Operator adaptor for TSP
*
*********************************************************************************/

#ifndef OFEC_INTERPRETER_TSP_H
#define OFEC_INTERPRETER_TSP_H

#include"../../../template/combination/sequence/sequence_interpreter.h"
#include"../../../template/combination/sequence/sequence_solution.h"
#include"../../../../../core/algorithm/individual.h"

namespace ofec {
	class IndividualTSP : public Individual<VarVec<int>>, public SequenceSolution {
	protected:
		std::vector<bool> m_feasible_citys;

	public:
		IndividualTSP(size_t num_obj, size_t num_con, size_t num_var) :
			Individual(num_obj, num_con, num_var), SequenceSolution(), m_feasible_citys(num_var, false) {}

		IndividualTSP &operator =(const IndividualTSP &rhs) = default;
		const std::vector<bool> &feasible()const {
			return m_feasible_citys;
		}
		std::vector<bool> &feasible() {
			return m_feasible_citys;
		}
	};

	class InterpreterTSP : public sequence::Interpreter<IndividualTSP> {
	private:
		Real m_eps = 1e-5;
	public:
		using IndividualType = IndividualTSP;
	public:
		InterpreterTSP() = default;
		~InterpreterTSP() = default;

		virtual void initializeByProblem(int id_pro) override;
		virtual Real heursticInfo(int id_pro, int from, int to, int obj_idx = 0)const override;
		virtual Real heursticInfo(int id_pro, const IndividualType &pop, int to, int obj_idx = 0)const override;
		virtual IndividualType heuristicSol(int id_pro, int id_alg, int id_rnd)const override;
		virtual int  curPositionIdx(int id_pro, const IndividualType &sol)const override;
		virtual void stepInit(int id_pro, IndividualType &sol)const override;
		virtual void calNextFeasible(int id_pro, IndividualType &sol)const override;
		virtual bool stepFeasible(int id_pro, IndividualType &sol) const override;
		virtual void stepBack(int id_pro, IndividualType &sol)const override;
		virtual bool stepNext(int id_pro, IndividualType &sol, int next)const override;
		virtual void updateCurEdges(int id_pro, IndividualType &sol)const override;
		virtual void updateEdges(int id_pro, IndividualType &sol, bool all = true)const  override;
		virtual bool stepFinish(int id_pro, const IndividualType &sol)const override;
		virtual void stepFinal(int id_pro, IndividualType &sol)const override {}
		virtual void getNearestNeighbors(int id_pro, int len, int from_idx, std::vector<int> &neighbors)const override;
		virtual void fillterFeasible(int id_pro, IndividualType &sol, std::vector<int> &feasible)const override;
		//// to do
		//virtual void generateSamplesSolutions(int id_pro, int id_rnd, const SolBase& curSol, std::vector<std::unique_ptr<SolBase>>& sols)const override {}
		//virtual int evaluate(int id_pro, int id_alg, int id_rnd, SolBase& curSol, bool effective = true)const override { return curSol.evaluate(id_pro, id_alg, effective); }

	protected:
		virtual void clearInfo(IndividualType &sol)const;
	protected:
		// the heuristic infomations from the start pos;
		std::vector<Real> m_start_pos_heuristic;
		int m_city_size = 0;
	};
}

#endif // !OFEC_INTERPRETER_TSP_H
