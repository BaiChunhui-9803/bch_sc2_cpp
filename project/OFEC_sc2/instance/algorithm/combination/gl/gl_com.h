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


*************************************************************************/


#ifndef GL_SEQ_H
#define GL_SEQ_H


#include"gl_pop_seq.h"
#include"../../template/combination/sequence/sequence_algorithm.h"
#include"../../template/combination/sequence/sequence_interpreter.h"
#include"../../template/combination/sequence/sequence_operator.h"

#include"../../template/combination/multi_population/distance_calculator.h"
#include"../../template/combination/multi_population/weight_calculator.h"
namespace ofec {
	template<typename TAdaptor>
	class GL_Seq : public AlgorithmSeq {
	public:
		using AdaptorType = typename TAdaptor;
		using OpType = typename TAdaptor::OpType;
		using IndividualType = typename TAdaptor::IndividualType;
		using InterpreterType = typename TAdaptor::InterpreterType;
	protected:
		PopGLSeq<AdaptorType> m_pop;
		WeightCalculator m_weight_calculator;
		DistanceCalculator<InterpreterType> m_distace_calculator;
		Real m_radius = 1.0;
		

	public:
		virtual void initialize_()override;
		virtual void run_()override;
		// Í¨¹ý Algorithm ¼Ì³Ð
		virtual void record() override {}



	};

	template<typename TSequenceOperator>
	inline void GL_Seq<TSequenceOperator>::initialize_()
	{
		AlgorithmSeq::initialize_();
		m_keep_candidates_updated = true;
		assignInterpreter<InterpreterType>(m_id_pro);
		assignOps<OpType>();
		m_distace_calculator.resize(getInterpreter<InterpreterType>());
		m_distace_calculator.initialize();
		m_pop.resize(std::get<int>(GET_PARAM(m_id_param).at("population size")),m_id_pro);
		m_pop.initialize(m_id_pro, m_id_alg, m_id_rnd);
		

	}

	template<typename TSequenceOperator>
	inline void GL_Seq<TSequenceOperator>::run_()
	{
		while (!terminating()) {
			m_pop.evolve(m_id_pro, m_id_alg, m_id_rnd);
			Real minObj(m_pop.begin()->get()->objective()[0]);
			Real maxObj(minObj);
			for (auto& it(m_pop.begin()); it != m_pop.end(); ++it) {
				minObj = std::min((*it)->objective()[0], minObj);
				maxObj = std::max((*it)->objective()[0], maxObj);
			}
			std::cout << "minOjb\t" << minObj << "\tmaxOjb\t" << maxObj << std::endl;
			
			{
				std::vector<Real> weight;
				{
					std::vector<SolBase*> pop(m_pop.size());
					for (int idx(0); idx < pop.size(); ++idx) {
						pop[idx] = &m_pop[idx];
					}
					m_weight_calculator.calWeight(m_id_pro, pop, weight);
				}

				{
					std::vector<IndividualType*> pop(m_pop.size());
					for (int idx(0); idx < pop.size(); ++idx) {
						pop[idx] = &m_pop[idx];
					}
					m_distace_calculator.addDistance(m_id_pro, getInterpreter<InterpreterType>(), pop, weight);
					m_distace_calculator.printInfo();
				}
				std::cout <<"numImprove\t" << m_pop.numImprove() << std::endl;
			}


			int pidx(0);
			for (int idx(0); idx < m_pop.size(); ++idx) {
				if (m_pop[idx].dominate(m_pop[pidx],m_id_pro)){
					pidx = idx;
				}
			}
			std::cout << "solution id\t" << pidx << std::endl;
			GET_PRO(m_id_pro).printfSolution(m_id_alg, m_pop[pidx]);
			m_pop.setRadius(m_radius);
			m_pop.shrinkPop(m_id_pro, m_id_alg, m_id_rnd);
			m_radius -= 0.01;
			if (m_radius <0.1) {
				m_radius = 0.1;
			}
			
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		//	GET_PRO(m_id_pro).showInfomations(m_id_alg);
		}
	}

}

#endif 