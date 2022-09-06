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




#ifndef OPERATOR_SEQUENCE_H
#define OPERATOR_SEQUENCE_H

#include<functional>
#include<vector>
#include"../../../../../core/problem/encoding.h"
#include"../../../../../core/definition.h"
#include "../../../../../utility/functional.h"
namespace ofec {


	namespace SEQUENCE {

		class SequenceOperatorBase {
		public:
			SequenceOperatorBase() = default;
			~SequenceOperatorBase() = default;
			virtual void initialize() {}
		};


		template<typename TInterpreter>
		class SequenceOperator : public SequenceOperatorBase {
		public:
			using IndividualType = typename TInterpreter::IndividualType;
			using InterpreterType = typename TInterpreter;
		public:
			SequenceOperator() = default;
			~SequenceOperator() = default;

			virtual int learn_from_local(
				const IndividualType& cur, int id_rnd, int id_pro,
				const InterpreterType& interpreter
				)const = 0;
			virtual int learn_from_global(
				const IndividualType& cur, int id_rnd, int id_pro,
				const InterpreterType& interpreter,
				const std::function<Real(const IndividualType& cur,int to)>& proFun 
			)const = 0;
			virtual int learn_from_other(
				const IndividualType& cur, int id_rnd, int id_pro,
				const InterpreterType& interpreter,
				const IndividualType& other
			)const = 0;

			virtual bool learn_from_other(
				IndividualType& cur, int id_rnd, int id_pro,
				const InterpreterType& interpreter,
				const IndividualType& other,
				Real radius
			)const = 0;

			virtual void localSearch(IndividualType& cur,
				int id_rnd, int id_pro, int id_alg, int totalEvals,int curType) {}
			virtual int evaluate(int id_pro, int id_alg, int id_rnd, IndividualType& curSol, bool effective = true)const = 0;

			virtual bool better(int id_pro,const IndividualType& a, const IndividualType& b) {
				return a.dominate(b, id_pro);
			}
			
		//	virtual int getIdx(int id_pro,std::vector<std::unique_ptr<IndividualType>>& curIdx,bool betterFlag=true);
		//	virtual int getIdx(int id_pro, std::vector<IndividualType>& curIdx, bool betterFlag = true);

		};

		//template<typename TInterpreter>
		//int SequenceOperator<TInterpreter>::getIdx(int id_pro, std::vector<std::unique_ptr<IndividualType>>& cur,bool betterFlag){
		//	if (betterFlag) {
		//		return calIdx(cur, [&](const std::unique_ptr<IndividualType>& a, const  std::unique_ptr<IndividualType>& b) {
		//			return better(id_pro, *a, *b);
		//		});
		//	}
		//	else {
		//		return calIdx(cur, [&](const std::unique_ptr<IndividualType>& a, const  std::unique_ptr<IndividualType>& b) {
		//			return !better(id_pro, *a, *b);
		//		});
		//	}
		//}

		//template<typename TInterpreter>
		//int SequenceOperator<TInterpreter>::getIdx(int id_pro, std::vector<IndividualType>& cur, bool betterFlag) {
		//	if (betterFlag) {
		//		return calIdx<IndividualType>(cur, [&](const IndividualType& a, const  IndividualType& b) {
		//			return better(id_pro, a, b);
		//		});
		//	}
		//	else {
		//		return calIdx(cur, [&](const IndividualType& a, const IndividualType& b) {
		//			return !better(id_pro, a, b);
		//		});
		//	}
		//}


	}



	//template<typename T>
	//int getWorstIdx(const std::vector<T>& indis) {
	//	if (T.size() == 0) return -1;
	//	int worstIdx(0);
	//	for (int curIdx(0); curIdx < T.size(); ++curIdx) {
	//		if (T[worstIdx].dominate(T[curIdx], m_id_pro)) {
	//			worstIdx = curIdx;
	//		}
	//	}
	//	return worstIdx;
	//}


	//template<typename T>
	//int getWorstIdx(int id_pro,const std::vector<T>& indis) {
	//	if (T.size() == 0) return -1;
	//	int worstIdx(0);
	//	for (int curIdx(0); curIdx < T.size(); ++curIdx) {
	//		if (T[worstIdx].dominate(T[curIdx], m_id_pro)) {
	//			worstIdx = curIdx;
	//		}
	//	}
	//	return worstIdx;
	//}


	//template<typename T>
	//int getWorstPtrIdx(int id_pro,const std::vector<std::unique_ptr<T>>& indis) {
	//	if (T.size() == 0) return -1;
	//	int worstIdx(0);
	//	for (int curIdx(0); curIdx < T.size(); ++curIdx) {
	//		if (T[worstIdx]->dominate(*T[curIdx], m_id_pro)) {
	//			worstIdx = curIdx;
	//		}
	//	}
	//	return worstIdx;
	//}

}

#endif // !OPERATOR_INTERPRETER_SEQ_H

