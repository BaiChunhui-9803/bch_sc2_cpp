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

Dorigo, M. (1996). "Ant system optimization by a colony of cooperating agents."
IEEE TRANSACTIONS ON SYSTEMS, MAN, AND CYBERNETICS.

*************************************************************************/
// Created: 7 Oct 2014
// Last modified:
// updated: by Yiya Diao in July  2021

#ifndef OFEC_POPULATION_ACO_H
#define OFEC_POPULATION_ACO_H

#include<vector>
#include"../../../../core/algorithm/population.h"
#include<functional>
#include<memory>

namespace ofec {
	template<typename T_ACO>
	class PopACO : public Population<typename T_ACO::IndividualType> {
	public:
		using IndividualType = typename T_ACO::IndividualType;
		using IntrepreterType = typename T_ACO::IntrepreterType;

	protected:
		IntrepreterType m_pro_interperter;
		T_ACO m_ACO_model;

	protected:
		virtual inline Real calFitness(int id_pro, IndividualType& sol) const {
			if (GET_PRO(id_pro).optMode()[0] == OptMode::kMaximize) {
				return   sol.objective(0);
			}
			else {
				return (1.0 / (1e-9 + sol.objective(0)));
			}
		}
		virtual int updateSolutions(int id_rnd,int id_pro, int id_alg);
		virtual int localSearch(int id_rnd, int id_pro, int id_alg) { return kNormalEval; }

	public:
		const IntrepreterType& getProInterpreter() {return m_pro_interperter;		}
		const std::vector<std::vector<Real>>& getMatrix() {			return m_ACO_model.getMatrix();		}	
		int evolve(int id_rnd,int id_pro, int id_alg) override;
		virtual void initializeParameters(int id_param, int id_rnd, int id_pro, int id_alg);
		virtual void initialize(int id_rnd,int id_pro,int id_alg); //a uniformly distributed initialization by default

	};

	template<typename T_ACO>
	inline int PopACO<T_ACO>::updateSolutions(int id_rnd,int id_pro, int id_alg) {
		for (auto& it : m_inds) {
			m_pro_interperter.stepInit(id_pro,*it);
			while (!m_pro_interperter.stepFinish(id_pro,*it)) {
				if (m_pro_interperter.stepFeasible(id_pro,*it)) {
					if (m_ACO_model.stepNext(id_rnd, id_pro, id_alg, m_pro_interperter, *it)) {
						m_ACO_model.localUpdatePheromone(id_rnd, id_pro, id_alg, m_pro_interperter, *it);
					}
					else {
						m_pro_interperter.stepBack(id_pro,*it);
					}
				}
				else {
					m_pro_interperter.stepBack(id_pro,*it);
				}
			}
			m_pro_interperter.stepFinal(id_pro,*it);
		}
		return localSearch(id_rnd,id_pro, id_alg);
	}

	template<typename T_ACO>
	inline int PopACO<T_ACO>::evolve(int id_rnd,int id_pro, int id_alg) {
		int tag = updateSolutions(id_pro, id_alg, id_rnd);
		if (tag != kNormalEval) {
			handleEvalTag(tag);
		}
		m_ACO_model.globalUpdatePheromone(id_rnd, id_pro, id_alg, m_pro_interperter, m_inds);
		return tag;
	}

	template<typename T_ACO>
	inline void PopACO<T_ACO>::initializeParameters(int id_param, int id_rnd, int id_pro,int id_alg) {
		Population<IndividualType>::clear();
		Population<IndividualType>::resize(std::get<int>(GET_PARAM(id_param).at("population size")), id_pro,GET_PRO(id_pro).numVariables());
		m_pro_interperter.initializeByProblem(id_pro);
		auto heuristic_sol(m_pro_interperter.heuristicSol(id_pro, id_alg, id_rnd));
//		updateFitness(id_pro, heuristic_sol);
		m_ACO_model.fitnessUpdate() =
			std::bind(&PopACO::calFitness, this,  std::placeholders::_1, std::placeholders::_2);
		m_ACO_model.initialize(id_param, id_rnd, id_pro, id_alg, m_pro_interperter);

		//m_ACO_model.setHeuristicFitness(heuristic_sol.fitness());
	}

	template<typename T_ACO>
	inline void PopACO<T_ACO>::initialize(int id_rnd,int id_pro,int id_alg) {
		Population::initialize(id_pro, id_rnd);
		for (auto& it : m_inds) {
			m_pro_interperter.stepFinal(id_pro,*it);
			m_pro_interperter.evaluate(id_pro, id_alg, id_rnd, *it);
		}
	}
}

#endif