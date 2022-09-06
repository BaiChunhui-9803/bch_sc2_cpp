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
// updated: Yiya Diao() 3 Dec 2018
//

#ifndef OFEC_ACO_MODEL
#define OFEC_ACO_MODEL

#include <vector>
#include "../../../../../core/definition.h"
#include "../../../../../core/instance_manager.h"
#include <functional>
#include <memory>

namespace ofec {
	template<typename TInterpreter>
	class ModelACO {
	public:
		using IndividualType = typename TInterpreter::IndividualType;
		using IntrepreterType = TInterpreter;
	protected:
		std::vector<std::vector<Real>> mvv_phero;
		std::function<Real(int id_pro, IndividualType &sol)> m_fitness_update = nullptr;
	protected:
		virtual Real getPro(const TInterpreter &interpreter, int pro_id, const IndividualType &cur, int to) = 0;
		virtual void initializePheromone() {
			for (auto &it : mvv_phero) {
				for (auto &it2 : it) {
					it2 = 1.0;
				}
			}
		}

	public:
		const std::vector<std::vector<Real>> &getMatrix() { return mvv_phero; }
		std::function<Real(int id_pro, IndividualType &sol)> &fitnessUpdate() { return m_fitness_update; }
		virtual void initialize(int id_param, int id_rnd, int id_pro, int id_alg, const TInterpreter &interpreter) {
			auto &mat(interpreter.getMatrixSize());
			mvv_phero.resize(mat.size());
			for (int idx(0); idx < mat.size(); ++idx) {
				mvv_phero[idx].resize(mat[idx]);
			}
			initializePheromone();
		}
		virtual void globalUpdatePheromone(int id_rnd, int id_pro, int id_alg, const TInterpreter &interpreter, std::vector<std::unique_ptr<IndividualType>> &pops) = 0;
		virtual void localUpdatePheromone(int id_rnd, int id_pro, int id_alg, const TInterpreter &interpreter, IndividualType &cur) {}
		virtual bool stepNext(int id_rnd, int id_pro, int id_alg, const TInterpreter &interpreter, IndividualType &cur) {
			interpreter.calNextFeasible(id_pro, cur);
			std::function<Real(int to)> nei_weight_fun = std::bind(&ModelACO::getPro, this, std::cref(interpreter), id_pro, std::cref(cur), std::placeholders::_1);
			auto next_step_iter(GET_RND(id_rnd).uniform.spinWheelIdx(cur.feasibleNeighbors().begin(), cur.feasibleNeighbors().end(), nei_weight_fun));
			if (next_step_iter == cur.feasibleNeighbors().end()) {
				return false;
			}
			return interpreter.stepNext(id_pro, cur, *next_step_iter);
		}
	};
}

#endif // ! OFEC_ACO_MODEL
