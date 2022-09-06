/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yiya Diao
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
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
* Operator adaptator of genetic learning algorithm
*
*********************************************************************************/

#ifndef OFEC_GL_ADAPTER_SEQ_H
#define OFEC_GL_ADAPTER_SEQ_H


#include"../../template/framework/gl/gl_adaptor.h"
#include"../../template/combination/sequence/sequence_algorithm.h"
#include "../../../../utility/functional.h"

namespace ofec {
	template<typename TSequenceOperator>
	class AdaptorGLSeq: public AdaptorGL<typename TSequenceOperator::IndividualType>{

	public:
		using OpType =  TSequenceOperator;
		using IndividualType = typename TSequenceOperator::IndividualType;
		using InterpreterType = typename TSequenceOperator::InterpreterType;
		
	protected:
		std::vector<std::vector<Real>> m_pro;
		enum class proUpdate { origin, min };
		proUpdate m_pro_update = proUpdate::min;
		int m_localSearchTimes = 30;
		Real m_radius = 1.0;
		IndividualType m_center;
		bool m_localSearch = true;

		std::list<std::unique_ptr<IndividualType>> m_discard_indis;
	//	std::list<Real> opVar;
	protected:
		void resetPro(const InterpreterType & interpreter) {
			auto& mat(interpreter.getMatrixSize());
			m_pro.resize(mat.size());
			for (int idx(0); idx < mat.size(); ++idx) {
				m_pro[idx].resize(mat[idx]);
				std::fill(m_pro[idx].begin(), m_pro[idx].end(), 0);
			}
		}

		void modifyWeightMin(const std::vector<Real>& weight) {
			Real minValue(weight.front());
			for (auto& it : weight) minValue = std::min(minValue, it);
			for (auto& it : m_pro) {
				for (auto& it2 : it) {
					it2 = std::max(it2, minValue);
				}
			}
		}
		std::list<std::unique_ptr<IndividualType>>& getDiscardIndis() {
			return m_discard_indis;
		}
	public:
		AdaptorGLSeq() { 
			m_alpha = 0.8;
		}
		virtual ~AdaptorGLSeq() {}
		const IndividualType& getCenter()const {
			return m_center;
		}

		virtual void initialze() {
			m_discard_indis.clear();
		}
		virtual void updateProbability(int id_pro, int id_alg,
			std::vector<std::unique_ptr<IndividualType>>& pop,
			const std::vector<Real>& weight,
			const std::vector<int>* index = nullptr)override;
		virtual void updateProbability(int id_pro, int id_alg,
			std::vector<IndividualType>& pop,
			const std::vector<Real>& weight,
			const std::vector<int>* index = nullptr)override;
		virtual void createSolution(int id_pro, int id_alg, int id_rnd,
			std::vector<std::unique_ptr<IndividualType>>& pop,
			std::vector<IndividualType>& offspring)override;
		virtual int updateSolution(int id_pro, int id_alg,
			std::vector<std::unique_ptr<IndividualType>>& pop,
			std::vector<IndividualType>& offspring, int& num_improve)override;

		virtual void setRadius(Real radius) {
			m_radius = radius;
		}
		virtual void shrinkPop(int id_pro, int id_alg, int id_rnd, std::vector<std::unique_ptr<IndividualType>>& pop);
		virtual void updateCenter(int id_pro, std::vector<std::unique_ptr<IndividualType>>& pop ) {
			auto& op(GET_ASeq(id_alg).getOp<TSequenceOperator>());
			int bestId=calIdx<std::unique_ptr<IndividualType>>(pop, [&](const std::unique_ptr<IndividualType>& a, const std::unique_ptr<IndividualType>& b) {
				return op.better(id_pro, *a, *b);
			});
			m_center = *pop[bestId];
			if (!opVar.empty()) {
				if (opVar.back() - m_center.objective()[0] < -0.01) {
					std::cout << "center\t" << m_center.objective()[0] << std::endl;
					int stop = -1;
				}
			}
			opVar.push_back(m_center.objective()[0]);
			
		}
	};



	template<typename TSequenceOperator>
	inline void AdaptorGLSeq<TSequenceOperator>::shrinkPop(int id_pro, int id_alg, int id_rnd, std::vector<std::unique_ptr<IndividualType>>& pop)
	{
		auto& interpreter(GET_ASeq(id_alg).getInterpreter<InterpreterType>());
		auto& op(GET_ASeq(id_alg).getOp<TSequenceOperator>());
		for (auto& it : pop) {
			if (op.learn_from_other(*it, id_rnd, id_pro, interpreter, m_center,m_radius)) {
			//	std::cout << "distance2center\t" << m_center.variableDistance(*it,id_pro) << std::endl;
				op.evaluate(id_pro, id_alg, id_rnd, *it);
				interpreter.stepFinal(id_pro, *it);
			}

		}
		//updateCenter(id_pro, pop);
	}


	template<typename TSequenceOperator>
	inline void AdaptorGLSeq<TSequenceOperator>::updateProbability(int id_pro, int id_alg, std::vector<std::unique_ptr<IndividualType>>& pop, const std::vector<Real>& weight, const std::vector<int>* index)
	{
		
		auto& interpreter(GET_ASeq(id_alg).getInterpreter<InterpreterType>());
		resetPro(interpreter);
		if (index != nullptr) {
			for (auto& pidx : *index) {
				auto& indi(pop[pidx]);
				interpreter.updateEdges(id_pro, *indi);
				for (auto& edge : indi->edges()) {
					m_pro[edge.first][edge.second] += weight[pidx];
				}
			}
		}
		else {
			for (int pidx(0); pidx < weight.size();++pidx) {
				auto& indi(pop[pidx]);
				interpreter.updateEdges(id_pro, *indi);
				for (auto& edge : indi->edges()) {
					m_pro[edge.first][edge.second] += weight[pidx];
				}
			}
			
		}
		if (m_pro_update == proUpdate::min) {
			modifyWeightMin(weight);
		}
		//auto& interpreter(GET_ASeq(id_alg).getInterpreter<InterpreterType>());

	}


	template<typename TSequenceOperator>
	inline void AdaptorGLSeq<TSequenceOperator>::updateProbability(int id_pro, int id_alg, std::vector<IndividualType>& pop, const std::vector<Real>& weight, const std::vector<int>* index)
	{

		auto& interpreter(GET_ASeq(id_alg).getInterpreter<InterpreterType>());
		resetPro(interpreter);
		if (index != nullptr) {
			for (auto& pidx : *index) {
				auto& indi(pop[pidx]);
				interpreter.updateEdges(id_pro, indi);
				for (auto& edge : indi.edges()) {
					m_pro[edge.first][edge.second] += weight[pidx];
				}
			}
		}
		else {
			for (int pidx(0); pidx < weight.size(); ++pidx) {
				auto& indi(pop[pidx]);
				interpreter.updateEdges(id_pro, indi);
				for (auto& edge : indi.edges()) {
					m_pro[edge.first][edge.second] += weight[pidx];
				}
			}

		}
		if (m_pro_update == proUpdate::min) {
			modifyWeightMin(weight);
		}
		//auto& interpreter(GET_ASeq(id_alg).getInterpreter<InterpreterType>());

	}


	template<typename TSequenceOperator>
	inline void AdaptorGLSeq<TSequenceOperator>::createSolution(int id_pro, int id_alg, int id_rnd, std::vector<std::unique_ptr<IndividualType>>& pop, std::vector<IndividualType>& offspring)
	{
		auto& interpreter(GET_ASeq(id_alg).getInterpreter<InterpreterType>());
		auto& op(GET_ASeq(id_alg).getOp<TSequenceOperator>());

		for(int  offId(0);offId< offspring.size();++offId)
		{
			auto& it = offspring[offId];
			it.reset();
			interpreter.stepInit(id_pro, it);
			while (!interpreter.stepFinish(id_pro, it)) {
				int next(-1);
				if (interpreter.stepFeasible(id_pro, it)) {
					if (GET_RND(id_rnd).uniform.next() < m_alpha
						/*std::max(m_alpha,1.0-m_radius/it->variable().size())*/) {
						next = op.learn_from_other(it, id_rnd, id_pro, interpreter,*pop[offId]);
					}
					if (next == -1) {
						auto weightFun = [&](const IndividualType& indi, int to) {
							return m_pro[interpreter.curPositionIdx(id_pro, indi)][to];
						};
						next = op.learn_from_global(it, id_rnd, id_pro, interpreter, weightFun);
					}
				}
				if (next != -1) {
					interpreter.stepNext(id_pro, it, next);
				}
				else {
					interpreter.stepBack(id_pro, it);
				}
			}
			op.learn_from_other(it, id_rnd, id_pro, interpreter, m_center, m_radius);
			interpreter.stepFinal(id_pro, it);
		}
		
	}
	template<typename TSequenceOperator>
	inline int AdaptorGLSeq<TSequenceOperator>::updateSolution(int id_pro, int id_alg, std::vector<std::unique_ptr<IndividualType>>& pop, std::vector<IndividualType>& offspring, int& num_improve)
	{

		auto& op(GET_ASeq(id_alg).getOp<TSequenceOperator>());
		auto& DNStrategy = GET_ASeq(id_alg).getDNstrategy();
		int rf = 0;
		if (GET_PRO(id_pro).hasTag(ProTag::DOP)) {
			std::vector<Real> origin_objs;
			for (int pidx(0); pidx < pop.size(); ++pidx) {
				origin_objs = pop[pidx]->objective();
				rf != op.evaluate(id_pro, id_alg, GET_ALG(id_alg).idRandom(), *pop[pidx]);
				{
					if (DNStrategy.judgeChange(id_pro, origin_objs, pop[pidx]->objective())) {
						pop[pidx]->setLocalSearch(false);
					}
				}
			//	std::cout << "obj different\t" << origin_objs.front() - pop[pidx]->objective().front() << std::endl;
			}
		}
		for (int pidx(0); pidx < pop.size(); ++pidx) {
		//	if (offspring[pidx].same(*pop[pidx], id_pro))continue;
			rf != op.evaluate(id_pro, id_alg, GET_ALG(id_alg).idRandom(), offspring[pidx]);
		}


		// getBestIndex
		{

			int popBestIndex = calIdx<std::unique_ptr<IndividualType>>(pop, [&](const std::unique_ptr<IndividualType>& a, const std::unique_ptr<IndividualType>& b) {
				return op.better(id_pro, *a, *b);
			});
			int offBestIndex = calIdx<IndividualType>(offspring, 
				[&](const IndividualType& a, const IndividualType& b) {
				return op.better(id_pro, a, b);
			});

			if (op.better(id_pro, *pop[popBestIndex], offspring[offBestIndex])) {
				m_center = *pop[popBestIndex];
			}
			else {
				m_center = offspring[offBestIndex];
			}
			if (!opVar.empty()) {
				if (opVar.back() - m_center.objective()[0] < -0.01) {
					std::cout << "center\t" << m_center.objective()[0] << std::endl;
					int stop = -1;
				}
			}
			opVar.push_back(m_center.objective()[0]);
		}
		for (auto& indi : offspring) {
			indi.setLocalSearch(false);
		}
		num_improve = 0;
		for (int pidx(0); pidx < pop.size(); ++pidx) {
			if (offspring[pidx].same(*pop[pidx], id_pro))continue;
			//rf!= op.evaluate(id_pro, id_alg, GET_ALG(id_alg).idRandom(), offspring[pidx]);
			Real rPop(pop[pidx]->variableDistance(m_center, id_pro));
			Real rOff(offspring[pidx].variableDistance(m_center, id_pro));
			pop[pidx]->setImproved(false);
			if (rPop <= m_radius && rOff <= m_radius) {
				if(op.better(id_pro, offspring[pidx], *pop[pidx]))
				 {
					*pop[pidx] = offspring[pidx];
					pop[pidx]->setImproved(true);
					num_improve++;
				}
			}
			else {
			//	std::cout << "rPopRadius\t" << rPop << "\trOff\t" << rOff << std::endl;
				if (rOff < rPop) {
					*pop[pidx] = offspring[pidx];
					if (op.better(id_pro, offspring[pidx], *pop[pidx])) {
						pop[pidx]->setImproved(true);
						num_improve++;
					}
				}
			}

		}

		if (m_localSearch) {
			int localSearchId(-1);
			for (int pidx(0); pidx < pop.size(); ++pidx) {
				if (!pop[pidx]->flagLocalSearch()) {
					if (localSearchId == -1 || 
						op.better(id_pro, *pop[pidx], *pop[localSearchId])) {
						localSearchId = pidx;
					}
				}
			}
			if (localSearchId != -1) {
				std::vector<Real> origin_objs(pop[localSearchId]->objective());
				op.localSearch(*pop[localSearchId], GET_ALG(id_alg).idRandom(), id_pro, id_alg, m_localSearchTimes, 0);
				if (DNStrategy.judgeChange(id_pro, origin_objs, pop[localSearchId]->objective())) {
					pop[localSearchId]->setLocalSearch(false);
				}
				else pop[localSearchId]->setLocalSearch(true);
			}

		}

		return rf;
	}
	

	
	

}

#endif



