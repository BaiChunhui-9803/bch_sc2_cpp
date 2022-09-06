#include "one_max.h"
#include "../../../../core/instance_manager.h"
#include "../../../../core/problem/solution.h"

namespace ofec {
	void OneMax::initialize_() {
		Problem::initialize_();
		auto & v = GET_PARAM(m_id_param);
		m_num_vars = std::get<int>(v.at("number of variables"));
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMaximize;
		m_optima.clear();
		m_optima.appendVar(VarVec<int>(std::vector<int>(m_num_vars, 1)));
		m_optima.appendObj(m_num_vars);
	}

	void OneMax::evaluate_(SolBase &s, bool effective) {
		VarVec<int> &x = dynamic_cast<Solution<VarVec<int>> &>(s).variable();
		std::vector<Real> &obj = dynamic_cast<Solution<VarVec<int>> &>(s).objective();
		for (int i = 0; i < m_num_objs; i++)
			obj[i] = 0;
		for (int n = 0; n < m_num_objs; n++)
			for (size_t i = 0; i < m_num_vars; i++)
				obj[n] += x[i];
	}

	bool OneMax::isValid(const SolBase &s) {
		if (!m_if_valid_check) return true;
		const auto &x = dynamic_cast<const Solution<VarVec<int>> &>(s).variable();
		for (int i = 0; i < m_num_vars; i++) {
			if (x[i] != 0 && x[i] != 1)
				return false;
		}
		return true;
	}

	void OneMax::initSolution(SolBase &s, int id_rnd) const {
		VarVec<int> &x = dynamic_cast<Solution<VarVec<int>> &>(s).variable();
		for (size_t i = 0; i < m_num_vars; i++)
			if (GET_RND(id_rnd).uniform.next() < 0.5)
				x[i] = 0;
			else x[i] = 1;
	}
	
	bool OneMax::same(const SolBase &s1, const SolBase &s2) const {
		const auto &x1 = dynamic_cast<const Solution<VarVec<int>> &>(s1).variable();
		const auto &x2 = dynamic_cast<const Solution<VarVec<int>> &>(s2).variable();
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				return false;
		return true;
	}

	Real OneMax::variableDistance(const SolBase &x1, const SolBase &x2) const {
		const auto &x1_ = dynamic_cast<const Solution<VarVec<int>> &>(x1).variable();
		const auto &x2_ = dynamic_cast<const Solution<VarVec<int>> &>(x2).variable();
		Real dis = 0;
		for (int i = 0; i < m_num_vars; i++)
			if (x1_[i] != x2_[i])
				dis++;
		return dis;
	}

	Real OneMax::variableDistance(const VarBase &s1, const VarBase &s2) const {
		const auto &x1 = dynamic_cast<const VarVec<int>&>(s1);
		const auto &x2 = dynamic_cast<const VarVec<int>&>(s2);
		Real dis = 0;
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}

	void OneMax::updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (candidates.empty())
			candidates.emplace_back(new Solution<VarVec<int>>(dynamic_cast<const Solution<VarVec<int>>&>(sol)));
		else if (sol.dominate(*candidates.front(), m_id_pro))
			candidates.front().reset(new Solution<VarVec<int>>(dynamic_cast<const Solution<VarVec<int>>&>(sol)));
	}

	size_t OneMax::numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (m_optima.isObjectiveGiven()
			&& !candidates.empty()
			&& candidates.front()->objectiveDistance(m_optima.objective(0)) < m_objective_accuracy)
			return 1;
		else
			return 0;
	}

	int OneMax::updateEvalTag(SolBase &s, int id_alg, bool effective_eval) {
		if (isValid(s)) return EvalTag::kNormalEval;
		else return EvalTag::kInfeasible;
	}
}