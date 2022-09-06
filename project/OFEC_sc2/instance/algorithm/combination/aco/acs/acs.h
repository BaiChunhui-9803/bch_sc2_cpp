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

#ifndef OFEC_ACS_H
#define OFEC_ACS_H

#include "../../../template/classic/aco/aco_model.h"
#include "../../../../../core/instance_manager.h"
#include "../../../../../utility/functional.h"
#include <cmath>

namespace ofec {
	template<typename TInterpreter>
	class ACS : public ModelACO<TInterpreter> {
	public:
		using IndividualType = typename TInterpreter::IndividualType;
		using IntrepreterType = TInterpreter;

	protected:
		Real m_beta = 2.0;
		Real m_Q = 0.9;
		Real m_evaporate_alpha = 0.9;
		Real m_rho = 0.9;
		std::vector<std::unique_ptr<IndividualType>> m_best_sols;
		Real m_t = 1.0;
		int m_neighbor_size = 0;

	private:
		std::vector<std::vector<bool>> m_best_sols_edges;

	protected:
		Real getPro(const TInterpreter& interpreter, int pro_id, const IndividualType& cur, int to) override {
			return  mvv_phero[interpreter.curPositionIdx(pro_id, cur)][to] * pow(interpreter.heursticInfo(pro_id, cur, to), m_beta);
		}

	public:
		void initialize(int id_param, int id_rnd, int id_pro, int id_alg, const TInterpreter& interpreter) override {
			ModelACO::initialize(id_param, id_rnd, id_pro, id_alg, interpreter);
			auto& v = GET_PARAM(id_param);
			if (v.find("ACS_beta") != v.end()) {
				m_beta = std::get<Real>(v.at("ACS_beta"));
			}
			else m_beta = 5.0;
			if (v.find("ACS_Q") != v.end()) {
				m_Q = std::get<Real>(v.at("ACS_Q"));
			}
			else m_Q = 100.; 
			if (v.find("ACS_rho") != v.end()) {
				m_rho = std::get<Real>(v.at("ACS_rho"));
			}
			else m_rho = 0.5;
			m_best_sols.clear();
			auto& mat(interpreter.getMatrixSize());
			auto sol(interpreter.heuristicSol(id_pro, id_alg, id_rnd));

			m_t = m_fitness_update(id_pro,sol) / static_cast<Real>(mat.size());
			m_best_sols_edges.resize(mat.size());
			for (int idx(0); idx < m_best_sols_edges.size(); ++idx) {
				m_best_sols_edges[idx].resize(mat[idx]);
				std::fill(m_best_sols_edges[idx].begin(), m_best_sols_edges[idx].end(), false);
			}			
		}

		void globalUpdatePheromone(int id_rnd, int id_pro, int id_alg, const TInterpreter& interpreter, std::vector<std::unique_ptr<IndividualType>>& pops) override {
			if (pops.empty())return;
			Real fitness(0);
			bool insertSol(false);
			for (auto& it : pops) {
				fitness = m_fitness_update(id_pro, *it);
				if (m_best_sols.empty()) {
					insertSol = true;
				}
				else if (m_best_sols.front()->fitness() < fitness) {
					m_best_sols.clear();
					insertSol = true;
				}
				else if (m_best_sols.front()->fitness() == fitness) {
					insertSol = true;
				}
				if (insertSol) {
					m_best_sols.emplace_back(new IndividualType(*it));
					m_best_sols.back()->setFitness(fitness);
				}
			}
			for (auto& it : m_best_sols_edges) {
				std::fill(it.begin(), it.end(), false);
			}
			for (auto& best_iter : m_best_sols) {
				for (auto& edge : best_iter->edges()) {
					m_best_sols_edges[edge.first][edge.second] = true;
				}
			}

			for (int dim1(0); dim1 < mvv_phero.size(); ++dim1) {
				for (int dim2(0); dim2 < mvv_phero[dim1].size(); ++dim2) {
					if(m_best_sols_edges[dim1][dim2])
					mvv_phero[dim1][dim2] = m_rho * mvv_phero[dim1][dim2] + m_Q* m_best_sols.front()->fitness();
					else {
						mvv_phero[dim1][dim2] *= m_rho;
					}
				}
			}

		}

		void localUpdatePheromone(int id_rnd, int id_pro, int id_alg, const TInterpreter& interpreter, IndividualType& cur) override {
			interpreter.updateCurEdges(id_pro, cur);
			// local update pheromone
			for (auto& edge : cur.currentEdges()) {
				mvv_phero[edge.first][edge.second] = m_evaporate_alpha * mvv_phero[edge.first][edge.second] + (1.0 - m_evaporate_alpha) * m_t;
			}
		}

		bool stepNext(int id_rnd, int id_pro, int id_alg, const TInterpreter& interpreter, IndividualType& cur) override {
			interpreter.calNextFeasible(id_pro, cur);
			std::function<Real(int to)> nei_weight_fun = std::bind(&ACS::getPro, this, std::cref(interpreter), id_pro, std::cref(cur), std::placeholders::_1);
			auto next_step_iter(cur.feasibleNeighbors().end());
			if (GET_RND(id_rnd).uniform.next() < m_Q) {
				next_step_iter = (GET_RND(id_rnd).uniform.greedyRandom(cur.feasibleNeighbors().begin(), cur.feasibleNeighbors().end(), nei_weight_fun));
			}
			else {
				next_step_iter = (GET_RND(id_rnd).uniform.spinWheelIdx(cur.feasibleNeighbors().begin(), cur.feasibleNeighbors().end(), nei_weight_fun));

			}
			if (next_step_iter == cur.feasibleNeighbors().end()) {
				return false;
			}
			return interpreter.stepNext(id_pro, cur, *next_step_iter);
		}
	};
}

#endif