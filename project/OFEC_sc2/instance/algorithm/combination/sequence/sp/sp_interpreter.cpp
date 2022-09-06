#include"sp_interpreter.h"
#include "../../../../../instance/problem/combination/selection_problem/selection_problem.h"
using namespace ofec;
using namespace SP;

void OFEC::SP_Interpreter::initializeByProblem(int id_pro)
{
	m_start_state_idx = 0;
	auto& map(GET_DYN_SP(id_pro).get_map());
	m_dim_size = map.m_stations.size();
	m_cur2idx.resize(map.m_stations.size());
	m_pos_size = 1;
	for (int dim(0); dim < map.m_stations.size(); ++dim) {
		m_cur2idx[dim].resize(map.m_stations[dim].size());
	}
	for (int idx(0); idx+1 < map.m_stations.size(); ++idx) {
		m_pos_size += map.m_stations[idx].size();
	}

	m_matrix_size.resize(m_pos_size);
	m_idx2cur.resize(m_pos_size);
	int idx(0);
	m_matrix_size[idx] = map.m_stations.front().size();
	m_idx2cur[idx] = std::make_pair(-1, -1);
	++idx;
	for (int x(0); x+1 < map.m_stations.size(); ++x) {
		for (int y(0); y < map.m_stations[x].size(); ++y) {
			m_idx2cur[idx] = std::make_pair(x, y);
			m_cur2idx[x][y] = idx;
			m_matrix_size[idx] = map.m_stations[x + 1].size();
			++idx;
		}
	}
	for (auto& it : m_cur2idx.back()) it = -1;
	
}

OFEC::Real OFEC::SP_Interpreter::heursticInfo(int id_pro, int from, int to, int obj_idx) const
{
	return 1.0/(GET_DYN_SP(id_pro).get_heuristic_info(m_idx2cur[from].first, m_idx2cur[from].second, to)+ SelectionProblem::m_eps);
}

OFEC::Real OFEC::SP_Interpreter::heursticInfo(int id_pro, const IndividualType& indi, int to, int obj_idx) const
{

	Real heu_info(m_eps);
	int cur_pos(curPositionIdx(id_pro,indi));
	if (cur_pos >= 0) {
		heu_info += GET_DYN_SP(id_pro).get_heuristic_info(indi.variable().vect(),indi.getLoc(),to);
	}
	return heu_info;
}

OFEC::SP_Interpreter::IndividualType OFEC::SP_Interpreter::heuristicSol(int id_pro, int id_alg, int id_rnd) const
{
	IndividualType sol;
	stepInit(id_pro, sol);
	while (!stepFinish(id_pro,sol)) {                                                                          (-1);
		Real maxH(-1e9), curH(0);
		std::vector<int> nei;
		int curPos(curPositionIdx(id_pro,sol));

		for (int to(0); to < m_matrix_size[curPos]; ++to) {
			curH = GET_DYN_SP(id_pro).get_heuristic_info(sol.variable().vect(), sol.getLoc(), to);
			if (maxH < curH) {
				maxH = curH;
				nei.clear();
				nei.push_back(maxH);
			}
			else if (maxH == curH) {
				nei.push_back(to);
			}
		}
		if (nei.empty()) {
			break;
		}
		stepNext(id_pro, sol, nei[GET_RND(id_rnd).uniform.nextNonStd<int>(0,nei.size())]);
	}
	return std::move(sol);
}

int OFEC::SP_Interpreter::curPositionIdx(int id_pro, const IndividualType& sol) const
{
	if (sol.getLoc() == 0)return m_start_state_idx;
	else return m_cur2idx[sol.getLoc()-1][sol.variable()[sol.getLoc()-1]];
}

void OFEC::SP_Interpreter::stepInit(int id_pro, IndividualType& sol) const
{
	SequenceInterpreter::stepInit(id_pro, sol);
	sol.getLoc() = 0;
	sol.variable().resize(m_dim_size);
	sol.flagCurrentEdgesUpdated() = false;
	sol.flagEdgesUpdated() = false;
	sol.flagFeasibleNeighborsUpdated() = false;
	
}

void OFEC::SP_Interpreter::calNextFeasible(int id_pro, IndividualType& sol) const
{
	if (!sol.flagFeasibleNeighborsUpdated()) {
		int cur_idx(curPositionIdx(id_pro, sol));
		if (cur_idx >= 0) {
			sol.feasibleNeighbors().resize(m_matrix_size[cur_idx]);
			for (int idx(0); idx < sol.feasibleNeighbors().size(); ++idx) {
				sol.feasibleNeighbors()[idx] = idx;
			}
		}
		else {
			sol.feasibleNeighbors().clear();
		}
		sol.flagFeasibleNeighborsUpdated() = true;
	}

}

bool OFEC::SP_Interpreter::stepFeasible(int id_pro, IndividualType& sol) const
{
	return sol.getLoc() != m_dim_size;
}

void OFEC::SP_Interpreter::stepBack(int id_pro, IndividualType& sol) const
{
	if (sol.getLoc()) --sol.getLoc();
	sol.flagCurrentEdgesUpdated() = false;
	sol.flagEdgesUpdated() = false;
	sol.flagFeasibleNeighborsUpdated() = false;
}

bool OFEC::SP_Interpreter::stepNext(int id_pro, IndividualType& sol, int next) const
{
	if (sol.getLoc() < m_dim_size) {
		sol.variable()[sol.getLoc()++] = next;
		sol.flagCurrentEdgesUpdated() = false;
		sol.flagEdgesUpdated() = false;
		sol.flagFeasibleNeighborsUpdated() = false;
		return true;
	}
	return false;
	// not to deal with contrained problem here
	//if (next>=0&&sol.getLoc() < m_dim_size && (!GET_DYN_SP(id_pro).get_map().m_stations[sol.getLoc()].empty())&&GET_DYN_SP(id_pro).get_map().m_stations[sol.getLoc()].size()<next) 
	//{
	//	sol.variable()[sol.getLoc()++] = next;
	//    return true;
	//}

	//return false;
}

void OFEC::SP_Interpreter::updateCurEdges(int id_pro, IndividualType& sol) const
{
	if (!sol.flagCurrentEdgesUpdated()) {
		int from(m_start_state_idx);
		if (sol.getLoc()) from = m_cur2idx[sol.getLoc() - 1][sol.variable()[sol.getLoc() - 1]];
		int to = sol.variable()[sol.getLoc()];
		sol.currentEdges().resize(1);
		sol.currentEdges().front() = std::make_pair(from, to);
		sol.flagCurrentEdgesUpdated() = true;
	}
}

void OFEC::SP_Interpreter::updateEdges(int id_pro, IndividualType& sol, bool all ) const
{
	if (!sol.flagEdgesUpdated()) {
		int loc = sol.getLoc();
		if (all) {
			loc = sol.variable().size();
		}

		int from(m_start_state_idx), to(-1);
		sol.edges().resize(loc);
		int idx(0);
		while (true) {
			sol.edges()[idx] = std::make_pair(from, sol.variable()[idx]);
			from = m_cur2idx[idx][sol.variable()[idx]];
			if (++idx == loc)break;
		}
		sol.flagEdgesUpdated() = true;
	}
}

bool OFEC::SP_Interpreter::stepFinish(int id_pro,const IndividualType& sol) const
{
	return sol.getLoc() == m_dim_size;
}

void OFEC::SP_Interpreter::stepFinal(int id_pro, IndividualType& sol) const
{
	sol.getLoc() = m_dim_size;
	sol.flagCurrentEdgesUpdated() = false;
	sol.flagEdgesUpdated() = false;
	sol.flagFeasibleNeighborsUpdated() = false;
}

void OFEC::SP_Interpreter::getNearestNeighbors(int id_pro, int len, int from_idx, std::vector<int>& neighbors) const
{
	std::vector<std::pair<int, double>> idx2dis(m_matrix_size[from_idx]);
	for (int to(0); to < idx2dis.size(); ++to) {
		idx2dis[to].first = to;
		idx2dis[to].second = heursticInfo(id_pro, from_idx, to);
	}
	std::sort(idx2dis.begin(), idx2dis.end(), [&](const std::pair<int,double>& a,const std::pair<int,double>& b) {
		return a.second > b.second;
	});
	len = std::min<int>(len, idx2dis.size());
	neighbors.resize(len);
	for (int idx(0); idx < len; ++idx) {
		neighbors[idx] = idx2dis[idx].first;
	}
}

void OFEC::SP_Interpreter::fillterFeasible(int id_pro, IndividualType& sol, std::vector<int>& feasible) const
{
	int cur_idx(curPositionIdx(id_pro, sol));
	if (cur_idx >= 0) {
		feasible.resize(m_matrix_size[cur_idx]);
		for (int idx(0); idx < feasible.size(); ++idx) {
			feasible[idx] = idx;
		}
	}
	else feasible.clear();
}


