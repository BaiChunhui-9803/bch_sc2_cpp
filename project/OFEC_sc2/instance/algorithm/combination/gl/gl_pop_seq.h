/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@google.com
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
* Framework of genetic learning (PopGL) algorithm
*
*********************************************************************************/

#ifndef OFEC_GL_SEQ_H
#define OFEC_GL_SEQ_H

#include  "../../template/framework/gl/gl_pop.h"


namespace ofec {
	template<typename TAdaptor>
	class PopGLSeq : public PopGL<typename TAdaptor::IndividualType> {
	public:
		using AdaptorType = typename TAdaptor;
		using OpType = typename TAdaptor::OpType;
		using IndividualType = typename TAdaptor::IndividualType;
		using InterpreterType = typename TAdaptor::InterpreterType;

	public:
        void initialize(int id_pro, int id_alg,int id_rnd);
		virtual int evolve(int id_pro, int id_alg, int id_rnd) override;
		virtual void setRadius(Real radius) {
			dynamic_cast<AdaptorType&>(*m_adaptor).setRadius(radius);
		}
		virtual void shrinkPop(int id_pro, int id_alg, int id_rnd);

	protected:
	//	int m_numImp = 0;

	};
	template<typename TSequenceOperator>
	void PopGLSeq<TSequenceOperator>::shrinkPop(int id_pro, int id_alg, int id_rnd){

		dynamic_cast<AdaptorType&>(*m_adaptor).shrinkPop(id_pro, id_alg, id_rnd, this->m_inds);
		for (auto& i : m_inds)
			updateBest(*i, id_pro);
	}


	template<typename TSequenceOperator>
	void PopGLSeq<TSequenceOperator>::initialize(int id_pro, int id_alg ,int id_rnd) {

		m_ms = UpdateScheme::ci;
		m_adaptor.reset(new AdaptorType());
		Population<IndividualType>::initialize(id_pro, id_rnd);
		auto& interpreter(GET_ASeq(id_alg).getInterpreter<InterpreterType>());
		auto& op(GET_ASeq(id_alg).getOp<OpType>());
		for (auto& it : m_inds) {
			interpreter.stepFinal(id_pro, *it);
			op.evaluate(id_pro, id_alg, id_rnd, *it);
		}

		dynamic_cast<AdaptorType&>(*m_adaptor).updateCenter(id_pro, m_inds);
		initializeMemory(id_pro,id_alg);
		for (int i = 0; i < this->size(); i++) {
			m_offspring.push_back(*this->m_inds[i]);
		}
	}
	template<typename TSequenceOperator>
	int PopGLSeq<TSequenceOperator>::evolve(int id_pro, int id_alg, int id_rnd) {
		updateMemory(id_pro, id_alg, true);

		m_adaptor->createSolution(id_pro,id_alg, id_rnd,m_inds, m_offspring);
		int rf = update(id_pro, id_alg);
		for (auto& i : m_inds)
				updateBest(*i, id_pro);

		m_iter++;
		return rf;
	}


	/*

	*/

}






#endif

