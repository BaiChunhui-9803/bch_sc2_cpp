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




#ifndef OPERATOR_SELECTION_PROBLEM_H
#define OPERATOR_SELECTION_PROBLEM_H
#include"../../../template/combination/sequence/sequence_operator.h"
#include"SP_interpreter.h"
namespace ofec {

		class SelectionProblemOperator: public SEQUENCE::SequenceOperator<SP_Interpreter> {
		public:
			enum class fitnessCalType { Objective, HLS };

		public:
			using IndividualType = SequenceOperator::IndividualType;
			using TInterpreter = SP_Interpreter;
		public:
			SelectionProblemOperator() = default;
            ~SelectionProblemOperator() = default;

			virtual int learn_from_local(
				const IndividualType& cur, int id_rnd, int id_pro,
				const TInterpreter& interpreter
			)const override;
			virtual int learn_from_global(
				const IndividualType& cur, int id_rnd, int id_pro,
				const TInterpreter& interpreter,
				const std::function<Real(const IndividualType& cur, int to)>& proFun
			)const override;
			virtual int learn_from_other(
				const IndividualType& cur, int id_rnd, int id_pro,
				const TInterpreter& interpreter,
				const IndividualType& other
			)const override;



			virtual bool learn_from_other(
				IndividualType& cur, int id_rnd, int id_pro,
				const InterpreterType& interpreter,
				const IndividualType& other,
				Real radius
			)const;
			

			virtual void localSearch(IndividualType& cur,
				int id_rnd, int id_pro, int id_alg, int totalEvals, int curType)override;
			virtual int evaluate(int id_pro, int id_alg, int id_rnd, IndividualType& curSol, bool effective = true)const override;
		protected:
			int m_sample_num = 100;
			fitnessCalType m_fitness_type = fitnessCalType::HLS;
		};

}

#endif // !OPERATOR_INTERPRETER_SEQ_H
