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
* class Algorithm is an abstract for all algorithms.
*
*********************************************************************************/
#ifndef OFEC_ALGORITHM_SEQ_H
#define OFEC_ALGORITHM_SEQ_H

#include"../../../../../core/algorithm/algorithm.h"
#include"sequence_interpreter.h"
#include"sequence_operator.h"
#include"../../../template/framework/dynamic_noisy/dynamic_noisy_strategy.h"
namespace ofec {


#define GET_ASeq(id_alg) dynamic_cast<AlgorithmSeq&>(InstanceManager::ms_instance_manager->getAlgorithm(id_pro))

	class AlgorithmSeq : virtual public Algorithm {
	protected:
		std::unique_ptr<SEQUENCE::SequenceOperatorBase> m_ops;
		std::unique_ptr<SEQUENCE::SequenceInterpreterBase> m_interpreters;
		DynamicNoisyStartegy m_DNstrategy;
	public:
		AlgorithmSeq& operator=(AlgorithmSeq&&) = default;
		virtual ~AlgorithmSeq() {}
		template<typename TOp>
		TOp& getOp() {
			return dynamic_cast<TOp&>(*m_ops.get());
		}
		template<typename TInterpreter>
		const TInterpreter& getInterpreter() {
			return dynamic_cast<const TInterpreter&>(*m_interpreters.get());
		}
		const DynamicNoisyStartegy & getDNstrategy() {
			return m_DNstrategy;
		}
	protected:
		template<typename TInterpreter>
		void assignInterpreter(int id_pro) {
			m_interpreters.reset(new TInterpreter());
			m_interpreters->initializeByProblem(id_pro);
		}

		template<typename TOp>
		void assignOps() {
			m_ops.reset(new TOp());
			m_ops->initialize();
		}

	};
}

#endif