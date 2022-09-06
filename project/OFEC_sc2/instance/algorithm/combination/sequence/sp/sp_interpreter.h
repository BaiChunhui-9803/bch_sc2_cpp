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




#ifndef OPERATOR_INTERPRETER_SP_H
#define OPERATOR_INTERPRETER_SP_H

#include"../../../template/combination/sequence/sequence_interpreter.h"
#include"../../../template/combination/sequence/sequence_solution.h"
#include"../../../../../core/algorithm/individual.h"

namespace ofec {
	class SP_individual : public Individual<VarVec<int>>, public SequenceSolution {
	public:
		template<typename ... Args>
		SP_individual(size_t num_obj, size_t num_con, Args&& ... args) :
			Individual(num_obj, num_con, std::forward<Args>(args)...), SequenceSolution(){}
		SP_individual() = default;


		void reset() {
			Individual::reset();
			SequenceSolution::reset();
		}
	};


	class SP_Interpreter : public SEQUENCE::SequenceInterpreter<SP_individual> {
	private:

	public:
		using IndividualType = typename SP_individual;
	public:
		SP_Interpreter() = default;
		~SP_Interpreter() = default;

		virtual void initializeByProblem(int id_pro) override;
		virtual Real heursticInfo(int id_pro, int from, int to, int obj_idx = 0)const override;
		virtual Real heursticInfo(int id_pro, const IndividualType& pop, int to, int obj_idx = 0)const override;
		virtual IndividualType heuristicSol(int id_pro, int id_alg, int id_rnd)const override;
		virtual int  curPositionIdx(int id_pro, const IndividualType& sol)const override;
		virtual void stepInit(int id_pro, IndividualType& sol)const override;
		virtual void calNextFeasible(int id_pro, IndividualType& sol)const override;
		virtual bool stepFeasible(int id_pro, IndividualType& sol) const override;
		virtual void stepBack(int id_pro, IndividualType& sol)const override;
		virtual bool stepNext(int id_pro, IndividualType& sol, int next)const override;
		virtual void updateCurEdges(int id_pro, IndividualType& sol)const override;
		virtual void updateEdges(int id_pro, IndividualType& sol, bool all = true)const  override;
		virtual bool stepFinish(int id_pro, const IndividualType& sol)const override;
		virtual void stepFinal(int id_pro, IndividualType& sol)const override;
		virtual void getNearestNeighbors(int id_pro, int len, int from_idx, std::vector<int>& neighbors)const override;
		virtual void fillterFeasible(int id_pro, IndividualType& sol, std::vector<int>& feasible)const override;

		
		// to do
		//virtual void generateSamplesSolutions(int id_pro, int id_rnd, const SolBase& curSol, std::vector<std::unique_ptr<SolBase>>& sols)const override;

	protected:
		std::vector<std::vector<int>> m_cur2idx;
		std::vector<std::pair<int, int>> m_idx2cur;
		int m_pos_size = 0;
		int m_dim_size = 0;
		int m_start_state_idx = 0;
		int m_size = 0;
		OFEC::Real m_eps = 1e-5;

	};



}

#endif // !OPERATOR_INTERPRETER_SEQ_H
