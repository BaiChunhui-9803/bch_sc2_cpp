#include "multi_dimensional_knapsack.h"
#include "../../../../core/instance_manager.h"
#include "../../../../core/global.h"
#include "../../../../core/problem/solution.h"
#include <fstream>
#include <numeric>

namespace ofec {
	void MultiDimensionalKnapsack::initialize_() {
		Problem::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_file_name = std::get<std::string>(v.at("dataFile1"));
		read_problem();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
	}

	void MultiDimensionalKnapsack::evaluate_(SolBase &s, bool effective) {
		VarVec<int> &x = dynamic_cast<Solution<VarVec<int>>&>(s).variable();
		std::vector<Real> &obj = dynamic_cast<Solution<VarVec<int>>&>(s).objective();
		int m = numInvalidConstraints(s);
		for (size_t i = 0; i < m_num_objs; ++i)
			obj[i] = 0;
		for (size_t n = 0; n < m_num_objs; ++n) {
			for (size_t i = 0; i < m_num_vars; ++i)
				obj[n] += mv_p[i] * x[i];
			obj[n] -= m * m_maxP;
		}
	}

	bool MultiDimensionalKnapsack::isValid(const SolBase &s) const {
		if (!m_if_valid_check) return true;
		const VarVec<int> &x = dynamic_cast<const Solution<VarVec<int>> &>(s).variable();
		for (int i = 0; i < m_num_vars; i++) {
			if (x[i] != 0 && x[i] != 1)
				return false;
		}
		return true;
	}

	void MultiDimensionalKnapsack::read_problem() {
		size_t i;
		std::string Line;
		std::ostringstream oss;
		std::ifstream infile;
		oss << static_cast<std::string>(g_working_dir);
		oss << "/instance/problem/combination/multi_dimensional_knapsack/data/" << m_file_name << ".mkp";
		infile.open(oss.str().c_str());
		if (!infile) {
			throw MyExcept("read Multidimensional Knapsack data error");
		}
		infile >> Line;
		m_num_vars = atoi(Line.c_str());
		infile >> Line;
		m_m = atoi(Line.c_str());
		infile >> Line;
		Real temp = atof(Line.c_str());
		if (temp != 0)
			m_optima.appendObj(temp);
		mv_p.resize(m_num_vars);
		mv_b.resize(m_m);
		mvv_r.resize(m_m);
		for (i = 0; i < m_m; i++) {
			mvv_r[i].resize(m_num_vars);
		}
		for (i = 0; i < m_num_vars; i++)
		{
			infile >> mv_p[i];
			if (i == 0) {
				m_maxP = mv_p[i];
			}
			else if (m_maxP < mv_p[i]) {
				m_maxP = mv_p[i];
			}
		}
		for (i = 0; i < m_m; i++) {
			for (int j = 0; j < m_num_vars; j++) {
				infile >> mvv_r[i][j];
			}
		}
		for (i = 0; i < m_m; i++) {
			infile >> mv_b[i];
		}
		infile.close();
		infile.clear();
	}

	int MultiDimensionalKnapsack::numInvalidConstraints(SolBase &s) const {
		const VarVec<int> &x = dynamic_cast<const Solution<VarVec<int>> &>(s).variable();
		int n = 0;
		Real sum;
		for (int i = 0; i < m_m; i++) {
			sum = 0;
			for (int j = 0; j < m_num_vars; j++) {
				sum += mvv_r[i][j] * x[j];
			}
			if (sum > mv_b[i]) n++;
		}
		return n;
	}

	Real MultiDimensionalKnapsack::getConstraintValue(const SolBase &s, std::vector<Real> &val) const {
		const VarVec<int> &x = dynamic_cast<const Solution<VarVec<int>> &>(s).variable();
		val.resize(m_m);
		Real sum = 0;
		for (int i = 0; i < m_m; ++i) {
			val[i] = 0;
			for (int j = 0; j < m_num_vars; ++j)
				val[i] += mvv_r[i][j] * x[j];
			val[i] = sum - mv_b[i] < 0 ? 0 : sum - mv_b[i];
		}
		return std::accumulate(val.begin(), val.end(), 0.0);
	}

	void MultiDimensionalKnapsack::initSolution(SolBase &s, int id_rnd) const {
		VarVec<int> &x = dynamic_cast<Solution<VarVec<int>> &>(s).variable();
		for (int i = 0; i < m_num_vars; ++i)
			x[i] = GET_RND(id_rnd).uniform.nextNonStd(0, 2);
		if (!isValid(s))
			throw MyExcept("error in @multi_dimensional_knapsack::initialize_solution() in multi_dimensional_knapsack.cpp");
	}

	bool MultiDimensionalKnapsack::same(const SolBase &s1, const SolBase &s2) const {
		const VarVec<int> &x1 = dynamic_cast<const Solution<VarVec<int>> &>(s1).variable();
		const VarVec<int> &x2 = dynamic_cast<const Solution<VarVec<int>> &>(s2).variable();
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				return false;
		return true;
	}

	Real MultiDimensionalKnapsack::variableDistance(const SolBase &s1, const SolBase &s2) const {
		const VarVec<int> &x1 = dynamic_cast<const Solution<VarVec<int>> &>(s1).variable();
		const VarVec<int> &x2 = dynamic_cast<const Solution<VarVec<int>> &>(s2).variable();
		Real dis = 0;
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}

	Real MultiDimensionalKnapsack::variableDistance(const VarBase &x1, const VarBase &x2) const {
		const VarVec<int> &x1_ = dynamic_cast<const VarVec<int>&>(x1);
		const VarVec<int> &x2_ = dynamic_cast<const VarVec<int>&>(x2);
		Real dis = 0;
		for (int i = 0; i < m_num_vars; i++)
			if (x1_[i] != x2_[i])
				dis++;
		return dis;
	}
}