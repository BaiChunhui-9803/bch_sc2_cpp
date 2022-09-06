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

#ifndef OFEC_ANT_SYSTEM_H
#define OFEC_ANT_SYSTEM_H

#include "../../../template/classic/aco/aco_model.h"
#include "../../../../../core/instance_manager.h"
#include <cmath>

namespace ofec {
	template<typename TInterpreter>
	class AS : public ModelACO<TInterpreter> {
	public:
		using IndividualType = typename TInterpreter::IndividualType;
		using IntrepreterType = TInterpreter;

	protected:
		Real m_alpha = 1.0;
		Real m_beta = 5.0;
		Real m_Q = 100.;
		Real m_rho = 0.5;

		Real getPro(const TInterpreter &interpreter, int pro_id, const IndividualType &cur, int to) override {
			return  pow(mvv_phero[interpreter.curPositionIdx(pro_id, cur)][to], m_alpha) * pow(interpreter.heursticInfo(pro_id, cur, to), m_beta);
		}

	public:
		void initialize(int id_param, int id_rnd, int id_pro, int id_alg, const TInterpreter &interpreter) override {
			ModelACO::initialize(id_param, id_rnd, id_pro, id_alg, interpreter);
			auto &v = GET_PARAM(id_param);
			if (v.find("AS_alpha") != v.end()) {
				m_alpha = std::get<Real>(v.at("AS_alpha"));
			}
			else m_alpha = 1.0;
			if (v.find("AS_beta") != v.end()) {
				m_beta = std::get<Real>(v.at("AS_beta"));
			}
			else m_beta = 5.0;
			if (v.find("AS_Q") != v.end()) {
				m_Q = std::get<Real>(v.at("AS_Q"));
			}
			else m_Q = 100.;
			if (v.find("AS_rho") != v.end()) {
				m_rho = std::get<Real>(v.at("AS_rho"));
			}
			else m_rho = 0.5;
		}

		void initializePheromone() override {
			for (auto &it : mvv_phero) {
				for (auto &it2 : it) {
					it2 = 1.0;
				}
			}
		}

		void globalUpdatePheromone(int id_rnd, int id_pro, int id_alg, const TInterpreter &interpreter, std::vector<std::unique_ptr<IndividualType>> &pops) override {
			for (auto &it : mvv_phero) {
				for (auto &it2 : it) {
					it2 *= m_rho;
				}
			}
			Real fitness(0);
			for (int idx(0); idx < pops.size(); ++idx) {
				fitness = m_fitness_update(id_pro, *pops[idx]);
				interpreter.updateEdges(id_pro, *pops[idx]);
				for (auto &cur_edge : pops[idx]->edges()) {
					mvv_phero[cur_edge.first][cur_edge.second] += m_Q * fitness;
				}
			}
		}
	};
}

#endif // ! OFEC_ANT_SYSTEM_H

