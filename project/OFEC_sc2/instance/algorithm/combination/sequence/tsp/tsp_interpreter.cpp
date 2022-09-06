#include "tsp_interpreter.h"
#include "../../../../../instance/problem/combination/travelling_salesman/travelling_salesman.h"
#include <iomanip>
#include <algorithm>

namespace ofec {
	void InterpreterTSP::initializeByProblem(int id_pro) {
		auto &tsp_cost(GET_TSP(id_pro).cost());
		m_city_size = tsp_cost.size();
		m_matrix_size.resize(m_city_size + 1);
		for (int from(0); from < m_city_size; ++from) {
			m_matrix_size[from] = tsp_cost[from].size();
		}
		m_matrix_size.back() = m_city_size;
		for (auto &it : m_matrix_size) {
			it = tsp_cost.size() + 1;
		}
		m_start_pos_heuristic.resize(m_city_size);
		for (auto &it : m_start_pos_heuristic) it = std::numeric_limits<Real>::max();
		for (int from(0); from < tsp_cost.size(); ++from) {
			for (int to(0); to < tsp_cost[from].size(); ++to) {
				if (from != to && tsp_cost[from][to] != 0) {
					m_start_pos_heuristic[from] = std::min(m_start_pos_heuristic[from], tsp_cost[from][to]);
				}
			}
		}
		for (auto &it : m_start_pos_heuristic) it = 1.0 / (m_eps + it);
	}

	Real InterpreterTSP::heursticInfo(int id_pro, int from, int to, int obj_idx) const {
		if (from == m_city_size) {
			return m_start_pos_heuristic[to];
		}
		else {
			return 1.0 / (m_eps + GET_TSP(id_pro).cost()[from][to]);
		}
	}

	Real InterpreterTSP::heursticInfo(int id_pro, const IndividualType &sol, int to, int obj_idx) const {
		if (sol.getLoc() == 0)
			return m_start_pos_heuristic[to];
		else {
			return 1.0 / (m_eps + GET_TSP(id_pro).cost()[sol.variable()[sol.getLoc() - 1]][to]);
		}
	}

	InterpreterTSP::IndividualType ofec::InterpreterTSP::heuristicSol(int id_pro, int id_alg, int id_rnd) const {
		IndividualType indi(GET_TSP(id_pro).numObjectives(), GET_TSP(id_pro).numConstraints(), GET_TSP(id_pro).numVariables());
		GET_TSP(id_pro).initSolutionNN(indi, id_rnd);
		indi.evaluate(id_pro, id_alg, true);
		return indi;
	}

	int InterpreterTSP::curPositionIdx(int id_pro, const IndividualType &sol) const {
		if (sol.getLoc() == 0) {
			return m_city_size;
		}
		else {
			return sol.variable()[sol.getLoc() - 1];
		}

	}

	void InterpreterTSP::stepInit(int id_pro, IndividualType &sol) const {
		sol.getLoc() = 0;
		clearInfo(sol);
		sol.feasible().resize(m_city_size);
		for (auto &it : sol.feasible())it = true;
	}

	void InterpreterTSP::calNextFeasible(int id_pro, IndividualType &sol) const {

		if (sol.flagFeasibleNeighborsUpdated())return;
		sol.feasibleNeighbors().clear();
		for (int idx(0); idx < m_city_size; ++idx) {
			if (sol.feasible()[idx]) {
				sol.feasibleNeighbors().push_back(idx);
			}
		}
		sol.flagFeasibleNeighborsUpdated() = true;
	}

	bool InterpreterTSP::stepFeasible(int id_pro, IndividualType &sol) const {
		return sol.getLoc() != m_city_size;

	}

	void InterpreterTSP::stepBack(int id_pro, IndividualType &sol) const {
		if (sol.getLoc() != 0) {
			sol.feasible()[sol.variable()[sol.getLoc() - 1]] = true;
			--sol.getLoc();
			clearInfo(sol);
		}
	}

	bool InterpreterTSP::stepNext(int id_pro, IndividualType &sol, int next) const {
		if (sol.feasible()[next]) {
			sol.feasible()[next] = false;
			sol.variable()[sol.getLoc()++] = next;
			sol.feasibleNeighbors().clear();
			sol.flagFeasibleNeighborsUpdated() = false;
			return true;
		}
		return false;
	}

	void InterpreterTSP::updateCurEdges(int id_pro, IndividualType &sol) const {
		if (sol.getLoc() == 0 || sol.flagCurrentEdgesUpdated())return;
		int to(sol.variable()[sol.getLoc() - 1]);
		int from(m_city_size);
		if (sol.getLoc() >= 2) {
			from = sol.variable()[sol.getLoc() - 2];
		}
		sol.currentEdges().resize(2);
		sol.currentEdges().front() = std::make_pair(from, to);
		sol.currentEdges().back() = std::make_pair(to, from);
		sol.flagCurrentEdgesUpdated() = true;
	}

	void InterpreterTSP::updateEdges(int id_pro, IndividualType &sol, bool all) const {
		if (sol.flagEdgesUpdated()) return;
		int from(m_city_size), to(0);
		int loc = sol.getLoc();
		if (all) {
			loc = sol.variable().size();
		}
		for (int to_idx(0); to_idx < loc; ++to_idx) {
			to = sol.variable()[to_idx];
			sol.edges().emplace_back(from, to);
			sol.edges().emplace_back(to, from);
			from = to;
		}
		sol.flagEdgesUpdated() = true;
	}

	bool InterpreterTSP::stepFinish(int id_pro, const IndividualType &sol) const {
		return sol.getLoc() == m_city_size;
	}

	void InterpreterTSP::getNearestNeighbors(int id_pro, int len, int from_idx, std::vector<int> &neighbors) const {
		neighbors.clear();
		len = std::min(m_city_size, len);
		neighbors.resize(m_city_size);
		for (int idx(0); idx < m_city_size; ++idx) {
			neighbors[idx] = idx;
		}
		if (from_idx == m_city_size) {
			std::sort(neighbors.begin(), neighbors.end(), [&](int a, int b) {
				return m_start_pos_heuristic[a] < m_start_pos_heuristic[b];
				});
		}
		else {
			auto &tsp_cost(GET_TSP(id_pro).cost());
			std::sort(neighbors.begin(), neighbors.end(), [&](int a, int b) {
				return tsp_cost[from_idx][a] > tsp_cost[from_idx][b];
				});
		}
		neighbors.resize(len);
	}

	void InterpreterTSP::fillterFeasible(int id_pro, IndividualType &sol, std::vector<int> &feasible) const {
		std::vector<int> temp(std::move(feasible));
		for (auto &it : temp) {
			if (sol.feasible()[it]) {
				feasible.push_back(it);
			}
		}
	}

	void InterpreterTSP::clearInfo(IndividualType &sol) const {
		sol.flagCurrentEdgesUpdated() = false;
		sol.flagEdgesUpdated() = false;
		sol.flagFeasibleNeighborsUpdated() = false;
	}
}
