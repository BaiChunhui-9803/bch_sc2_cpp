#include "quadratic_assignment.h"
#include "../../../../core/problem/solution.h"
#include "../../../../core/global.h"
#include "../../../../core/instance_manager.h"
#include <fstream>
#include <cstring>
#include <cstdlib>

namespace ofec {
	void QuadraticAssignment::initialize_() {
		Problem::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_file_name = std::get<std::string>(v.at("dataFile1"));
		readProblem();
		m_domain.resize(m_num_vars);
		for (size_t i = 0; i < m_num_vars; ++i)
			m_domain.setRange(0, m_num_vars - 1, i);
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;
	}

	void QuadraticAssignment::evaluate_(SolBase &s, bool effective) {
		VarVec<int> &x = dynamic_cast<Solution<VarVec<int>> &>(s).variable();
		std::vector<Real> &obj = dynamic_cast<Solution<VarVec<int>> &>(s).objective();
		for (size_t i = 0; i < m_num_objs; i++)
			obj[i] = 0;
		int row, col;
		for (size_t n = 0; n < m_num_objs; n++) {
			for (size_t i = 0; i < m_num_vars; i++) {
				row = x[i];
				for (size_t j = 0; j < m_num_vars; j++) {
					col = x[j];
					obj[n] += mvv_distance[i][j] * mvv_flow[row][col];
				}
			}
		}
	}

	bool QuadraticAssignment::isValid(const SolBase &s) const {
		if (!m_if_valid_check)
			return true;
		const VarVec<int> &x = dynamic_cast<const Solution<VarVec<int>> &>(s).variable();
		for (int i = 0; i < m_num_vars; i++)  //judge the range		
			if ((x[i]) < m_domain.range(i).limit.first || (x[i]) > m_domain.range(i).limit.second)
				return false;
		std::vector<int> flag(m_num_vars, 0);  //judge whether has the same gene
		int temp;
		for (int i = 0; i < m_num_vars; i++) {
			temp = x[i];
			flag[temp] = 1;
		}
		for (int i = 0; i < m_num_vars; i++)
			if (flag[i] == 0)
				return false;
		return true;
	}

	void QuadraticAssignment::readProblem() {
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#define	strtok_s strtok_r
#endif
		size_t i;
		std::string Line;
		char *Keyword = nullptr;
		const char *Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";
		std::ostringstream oss;
		std::ifstream infile;
		oss << static_cast<std::string>(g_working_dir);
		oss << "/instance/problem/combination/quadratic_assignment/data/" << m_file_name << ".qap";
		infile.open(oss.str().c_str());
		if (!infile) {
			throw MyExcept("read Quadratic Assignment data error");
		}
		char *savePtr;
		while (getline(infile, Line)) {
			if (!(Keyword = strtok_s((char *)Line.c_str(), Delimiters, &savePtr)))
				continue;
			for (i = 0; i < strlen(Keyword); i++)
				Keyword[i] = toupper(Keyword[i]);
			if (!strcmp(Keyword, "DIM")) {
				char *token = strtok_s(nullptr, Delimiters, &savePtr);
				m_num_vars = atoi(token);
			}
			else if (!strcmp(Keyword, "FLOW")) {
				mvv_flow.resize(m_num_vars);
				for (size_t i = 0; i < m_num_vars; i++)
					mvv_flow[i].resize(m_num_vars);
				for (size_t n = 0; n < m_num_vars; n++)
					for (i = 0; i < m_num_vars; i++)
						infile >> mvv_flow[n][i];
			}
			else if (!strcmp(Keyword, "DISTANCE")) {
				mvv_distance.resize(m_num_vars);
				for (size_t i = 0; i < m_num_vars; i++)
					mvv_distance[i].resize(m_num_vars);
				for (size_t n = 0; n < m_num_vars; n++)
					for (i = 0; i < m_num_vars; i++)
						infile >> mvv_distance[n][i];
			}
			else if (!strcmp(Keyword, "OPT_OBJ")) {
				char *token = strtok_s(nullptr, Delimiters, &savePtr);
				m_optima.appendObj(std::vector<Real>(1, atof(token)));
			}
			else if (!strcmp(Keyword, "OPT_SOLUTION")) {
				getline(infile, Line);
				std::istringstream ss(Line);
				VarVec<int> temp(m_num_vars);
				for (i = 0; i < m_num_vars; ++i) {
					ss >> temp[i];
				}
				m_optima.appendVar(temp);
			}
		}
		infile.close();
		infile.clear();
	}

	void QuadraticAssignment::initSolution(SolBase &s, int id_rnd) const {
		VarVec<int> &x = dynamic_cast<Solution<VarVec<int>>&>(s).variable();
		std::vector<int> temp;
		int i, pos, num = x.size();
		for (i = 0; i < num; i++)
			temp.push_back(int(i));
		for (i = 0; i < num; i++) {
			pos = int((num - 1 - i) * GET_RND(id_rnd).uniform.next());
			x[i] = temp[pos];
			temp[pos] = temp[num - 1 - i];
		}
		if (!isValid(s))
			throw MyExcept("error in @QuadraticAssignment::initialize_solution() in quadratic_assignment.cpp");
	}

	bool QuadraticAssignment::same(const SolBase &s1, const SolBase &s2) const {
		const VarVec<int> &x1 = dynamic_cast<const Solution<VarVec<int>> &>(s1).variable();
		const VarVec<int> &x2 = dynamic_cast<const Solution<VarVec<int>> &>(s2).variable();
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				return false;
		return true;
	}

	Real QuadraticAssignment::variableDistance(const SolBase &s1, const SolBase &s2) const {
		const VarVec<int> &x1 = dynamic_cast<const Solution<VarVec<int>> &>(s1).variable();
		const VarVec<int> &x2 = dynamic_cast<const Solution<VarVec<int>> &>(s2).variable();
		Real dis = 0;
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}

	Real QuadraticAssignment::variableDistance(const VarBase &x1, const VarBase &x2) const {
		const auto &x1_ = dynamic_cast<const VarVec<int>&>(x1);
		const auto &x2_ = dynamic_cast<const VarVec<int>&>(x2);
		Real dis = 0;
		for (int i = 0; i < m_num_vars; i++)
			if (x1_[i] != x2_[i])
				dis++;
		return dis;
	}

	void QuadraticAssignment::updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (candidates.empty())
			candidates.emplace_back(new Solution<VarVec<int>>(dynamic_cast<const Solution<VarVec<int>>&>(sol)));
		else if (sol.dominate(*candidates.front(), m_id_pro))
			candidates.front().reset(new Solution<VarVec<int>>(dynamic_cast<const Solution<VarVec<int>>&>(sol)));
	}

	size_t QuadraticAssignment::numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (m_optima.isObjectiveGiven()
			&& !candidates.empty()
			&& candidates.front()->objectiveDistance(m_optima.objective(0)) < m_objective_accuracy)
			return 1;
		else
			return 0;
	}
	
	int QuadraticAssignment::updateEvalTag(SolBase &s, int id_alg, bool effective_eval) {
		if (isValid(s)) return EvalTag::kNormalEval;
		else return EvalTag::kInfeasible;
	}
}