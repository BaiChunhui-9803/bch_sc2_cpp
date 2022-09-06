#include "../../../../core/problem/solution.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "metrics_mmop.h"
#include <algorithm>

namespace ofec {
	void MetricsMMOP::updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		switch (m_mb) {
		case ofec::MetricsMMOP::MeasureBy::kObj:
			updateByObj(sol, candidates); break;
		case ofec::MetricsMMOP::MeasureBy::kVar:
			updateByVar(sol, candidates); break;
		case ofec::MetricsMMOP::MeasureBy::kObjAndVar:
			updateByObjAndVar(sol, candidates); break;
		}	
	}

	size_t MetricsMMOP::numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		switch (m_mb) {
		case ofec::MetricsMMOP::MeasureBy::kObj:
			return numOptimaByObj(candidates);
		case ofec::MetricsMMOP::MeasureBy::kVar:
			return numOptimaByVar(candidates);
		case ofec::MetricsMMOP::MeasureBy::kObjAndVar:
			return numOptimaByObjAndVar(candidates);
		}		
	}

	bool MetricsMMOP::isSolved(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		return numOptimaFound(candidates) == m_optima.numberObjectives();
	}

	void MetricsMMOP::updateByObj(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (sol.objectiveDistance(m_optima.objective(0)) < m_objective_accuracy) {
			for (auto &c : candidates) {
				if (c->variableDistance(sol, m_id_pro) < m_variable_niche_radius)
					return;
			}
			candidates.emplace_back(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
		}
	}

	size_t MetricsMMOP::numOptimaByObj(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		std::list<SolBase *> opts_fnd;
		for (auto &c : candidates) {
			if (c->objectiveDistance(m_optima.objective(0)) < m_objective_accuracy) {
				bool is_new_opt = true;
				for (auto of : opts_fnd) {
					if (of->variableDistance(*c, m_id_pro) < m_variable_niche_radius) {
						is_new_opt = false;
						break;
					}
				}
				if (is_new_opt)
					opts_fnd.push_back(c.get());
			}
		}
		return opts_fnd.size();
	}

	void MetricsMMOP::updateByObjAndVar(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		std::vector<bool> is_opt_fnd(m_optima.numberObjectives(), false);
		Real dis_obj, dis_var;	
		for (auto &c : candidates) {
			for (size_t i = 0; i < m_optima.numberVariables(); i++) {
				if (is_opt_fnd[i])
					continue;
				dis_obj = c->objectiveDistance(m_optima.objective(i));
				dis_var = c->variableDistance(m_optima.variable(i), m_id_pro);
				if (dis_obj < m_objective_accuracy && dis_var < m_variable_niche_radius)
					is_opt_fnd[i] = true;
			}
		}
		for (size_t i = 0; i < m_optima.numberObjectives(); i++) {
			if (!is_opt_fnd[i]) {
				dis_obj = sol.objectiveDistance(m_optima.objective(i));
				dis_var = sol.variableDistance(m_optima.variable(i), m_id_pro);
				if (dis_obj < m_objective_accuracy && dis_var < m_variable_niche_radius) {
					candidates.emplace_back(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
					break;
				}
			}
		}
	}

	size_t MetricsMMOP::numOptimaByObjAndVar(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		size_t count = 0;
		Real dis_obj, dis_var;
		for (size_t i = 0; i < m_optima.numberObjectives(); i++) {
			for (auto &c : candidates) {
				dis_obj = c->objectiveDistance(m_optima.objective(i));
				dis_var = c->variableDistance(m_optima.variable(i), m_id_pro);
				if (dis_obj < m_objective_accuracy && dis_var < m_variable_niche_radius) {
					count++;
					break;
				}
			}
		}
		return count;
	}

	void MetricsMMOP::updateByVar(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		auto &s = dynamic_cast<const Solution<>&>(sol);
		if (candidates.empty()) {
			for (size_t i = 0; i < m_optima.numberVariables(); i++)
				candidates.emplace_back(new Solution<>(s));
			return;
		}
		auto it = candidates.begin();
		for (size_t i = 0; i < m_optima.numberVariables(); ++i, ++it) {
			if (variableDistance(s.variable(), m_optima.variable(i)) < (*it)->variableDistance(m_optima.variable(i), m_id_pro))
				(*it).reset(new Solution<>(s));
		}
	}

	size_t MetricsMMOP::numOptimaByVar(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		size_t num = 0;
		if (!candidates.empty()) {
			auto it = candidates.begin();
			for (size_t i = 0; i < m_optima.numberVariables(); ++i, ++it) {
				if ((*it)->objectiveDistance(m_optima.objective(i)) < m_objective_accuracy)
					num++;
			}
		}
		return num;
	}

	void MetricsMMOP::updateBestFixedNumSols(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates, size_t num_sols) const {
		bool flag_add = true;
		for (auto iter = candidates.begin(); iter != candidates.end();) {
			if (sol.variableDistance(**iter, m_id_pro) < m_variable_niche_radius) {
				if (sol.dominate(**iter, m_id_pro))
					iter = candidates.erase(iter);
				else {
					flag_add = false;
					break;
				}
			}
			else
				iter++;
		}
		if (flag_add) {
			if (candidates.size() < num_sols)
				candidates.emplace_back(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
			else {
				auto iter_worst = std::min_element(
					candidates.begin(), candidates.end(),
					[this](const std::unique_ptr<SolBase> &rhs1, const std::unique_ptr<SolBase> &rhs2) { return rhs1->dominate(*rhs2, this->m_id_pro); });
				(*iter_worst).reset(new Solution<>(dynamic_cast<const Solution<>&>(sol)));
			}
		}
	}
}