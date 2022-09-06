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
* Operator adaptor for combinatorial problem that can be classified into the sequence problem
*
*********************************************************************************/

#ifndef OFEC_SEQUENCE_INTERPRETER_H
#define OFEC_SEQUENCE_INTERPRETER_H

#include "../../../../../core/definition.h"
#include "../../../../../core/problem/encoding.h"
#include <vector>
#include <iostream>

// problem interpreter and operator for sequence problem
namespace ofec {
	namespace sequence {
		class InterpreterBase {
		public:
			InterpreterBase() = default;
			~InterpreterBase() = default;

			virtual void initializeByProblem(int id_pro) = 0;
			virtual void updateByProblem(int id_pro) {}
			const std::vector<int> &getMatrixSize() const {
				return m_matrix_size;
			}
			virtual bool optimalEdgeGiven(int id_pro)const {
				return false;
			}
			virtual void calOptEdges(int pro)const {};
		protected:
			std::vector<int> m_matrix_size;
		};

		template<typename TIndividual>
		class Interpreter : public InterpreterBase {
		public:
			using IndividualType = TIndividual;
		public:
			Interpreter() = default;
			~Interpreter() = default;

			virtual Real heursticInfo(int id_pro, int from, int to, int obj_idx = 0)const = 0;
			virtual Real heursticInfo(int id_pro, const IndividualType &pop, int to, int obj_idx = 0) const = 0;
			virtual IndividualType heuristicSol(int id_pro, int id_alg, int id_rnd)const = 0;
			virtual void getNearestNeighbors(int id_pro, int len, int from_idx, std::vector<int> &neighbors) const = 0;
			virtual int curPositionIdx(int id_pro, const IndividualType &sol)const = 0;
			virtual void stepInit(int id_pro, IndividualType &sol) const { sol.evaluateTimes() = 0; }
			virtual void calNextFeasible(int id_pro, IndividualType &sol) const = 0;
			virtual bool stepFeasible(int id_pro, IndividualType &sol) const = 0;
			virtual void fillterFeasible(int id_pro, IndividualType &sol, std::vector<int> &feasible) const = 0;
			virtual void stepBack(int id_pro, IndividualType &sol) const = 0;
			virtual bool stepNext(int id_pro, IndividualType &sol, int next) const = 0;
			virtual void updateCurEdges(int id_pro, IndividualType &sol) const = 0;
			virtual void updateEdges(int id_pro, IndividualType &sol, bool all = true) const = 0;
			virtual bool stepFinish(int id_pro, const IndividualType &sol) const = 0;
			virtual void stepFinal(int id_pro, IndividualType &sol) const = 0;
			virtual int evaluate(int id_pro, int id_alg, int id_rnd, SolBase &curSol, bool effective = true) const {
				return curSol.evaluate(id_pro, id_alg, effective);
			}
		};
	}
}

#endif // !OFEC_SEQUENCE_INTERPRETER_H
