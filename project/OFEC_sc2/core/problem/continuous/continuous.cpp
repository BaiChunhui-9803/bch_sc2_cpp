#include "continuous.h"
#include "../../../utility/functional.h"
#include "../solution.h"
#include <set>

namespace ofec {
	void Continuous::initialize_() {
		Problem::initialize_();
		m_num_vars = 0;
		m_optima.clear();
		m_domain_update = false;
		m_domain_area = 0;
		m_domain.clear();
		m_initial_domain.clear();
	}

	void Continuous::initSolution(SolBase& s, int id_rnd) const {
		VarVec<Real>& x = dynamic_cast<Solution<>&>(s).variable();
		for (int i = 0; i < m_num_vars; ++i) {
			if (m_initial_domain[i].limited)
				x[i] = GET_RND(id_rnd).uniform.nextNonStd(m_initial_domain[i].limit.first, m_initial_domain[i].limit.second);
			else {
				if (m_domain[i].limited)     // If m_initial_domain is not given, then use problem boundary as initialization range
					x[i] = GET_RND(id_rnd).uniform.nextNonStd(m_domain[i].limit.first, m_domain[i].limit.second);
				else                         // Else if the problem function has no boundary 
					x[i] = GET_RND(id_rnd).uniform.nextNonStd(-std::numeric_limits<Real>::max(), std::numeric_limits<Real>::max());
			}
		}
	}

	void Continuous::resizeVariable(size_t num_vars) {
		m_num_vars = num_vars;
		m_domain.resize(m_num_vars);
		m_initial_domain.resize(m_num_vars);
	}

	void Continuous::resizeObjective(size_t num_objs) {
		Problem::resizeObjective(num_objs);
		m_optima.resizeObjective(num_objs);
	}

	void Continuous::copy(const Problem& rhs) {
		// virtual inherit class do not need to copy
		//Problem::copy(rhs);
		auto& dcp = dynamic_cast<const Continuous&>(rhs);
		m_num_vars = dcp.m_num_vars;
		size_t num_vars = dcp.m_num_vars < m_num_vars ? dcp.m_num_vars : m_num_vars;
		for (size_t i = 0; i < num_vars; ++i) {
			m_domain[i] = dcp.m_domain[i];
			m_initial_domain[i] = dcp.m_initial_domain[i];
		}
		m_domain_update = dcp.m_domain_update;
		m_domain_area = dcp.m_domain_area;
		m_optima = dcp.m_optima;
	}

	bool Continuous::boundaryViolated(const SolBase &s) const {
		const VarVec<Real> &x = dynamic_cast<const Solution<>&>(s).variable();
		for (int i = 0; i < m_num_vars; ++i) {
			if (m_domain[i].limited) {
				if (x[i]<m_domain[i].limit.first || x[i]>m_domain[i].limit.second)
					return true;
			}
		}
		return false;
	}

	void Continuous::validateSolution(SolBase& s, Validation mode, int id_rnd)const {
		VarVec<Real>& x = dynamic_cast<Solution<>&>(s).variable();
		switch (mode) {
		case Validation::kIgnore:
			break;
		case Validation::kReinitialize:
			for (size_t j = 0; j < m_num_vars; ++j) {
				if (m_domain[j].limited)
					x[j] = GET_RND(id_rnd).uniform.nextNonStd(m_domain[j].limit.first, m_domain[j].limit.second);
			}
			break;
		case Validation::kRemap:
			for (size_t j = 0; j < m_num_vars; ++j) {
				Real l = m_domain[j].limit.first, u = m_domain[j].limit.second;
				if (m_domain[j].limited) {
					if (x[j] < l)
						x[j] = l + (u - l) * (l - x[j]) / (u - x[j]);
					else if (x[j] > u)
						x[j] = l + (u - l) * (u - l) / (x[j] - l);
				}
			}
			break;
		case Validation::kSetToBound:
			for (size_t j = 0; j < m_num_vars; ++j) {
				Real l = m_domain[j].limit.first, u = m_domain[j].limit.second;
				if (m_domain[j].limited) {
					if (x[j] < l)
						x[j] = l;
					else if (x[j] > u)
						x[j] = u;
				}
			}
			break;
		default:
			break;
		}
	}

	void Continuous::updateParameters() {
		Problem::updateParameters();
		if (m_num_vars > 0)
			m_params["number of variables"] = static_cast<int>(m_num_vars);
	}

	std::vector<std::pair<Real, Real>> Continuous::boundary() const {
		std::vector<std::pair<Real, Real>> boundary(m_domain.size());
		for (size_t j = 0; j < m_domain.size(); ++j)
			boundary[j] = m_domain.range(j).limit;
		return boundary;
	}

	void Continuous::setDomain(Real l, Real u) {
		if (m_domain.size() != m_num_vars)
			m_domain.resize(m_num_vars);
		for (size_t i = 0; i < m_domain.size(); ++i)
			m_domain.setRange(l, u, i);
		m_domain_update = true;
	}

	void Continuous::setDomain(const std::vector<std::pair<Real, Real>>& r) {
		size_t count = 0;
		for (auto& i : r) {
			m_domain.setRange(i.first, i.second, count++);
		}
		m_domain_update = true;
	}

	void Continuous::setInitialDomain(Real l, Real u) {
		for (size_t i = 0; i < m_domain.size(); ++i)
			m_initial_domain.setRange(l, u, i);
	}

	void Continuous::setInitialDomain(const std::vector<std::pair<Real, Real>>& r) {
		size_t count = 0;
		for (auto& i : r) {
			m_initial_domain.setRange(i.first, i.second, count++);
		}
	}

	Real Continuous::variableDistance(const SolBase& s1, const SolBase& s2) const {
		const VarVec<Real>& x1 = dynamic_cast<const Solution<>&>(s1).variable();
		const VarVec<Real>& x2 = dynamic_cast<const Solution<>&>(s2).variable();
		return euclideanDistance(x1.begin(), x1.end(), x2.begin());

	}

	Real Continuous::variableDistance(const VarBase& s1, const VarBase& s2) const {
		const auto& x1 = dynamic_cast<const VarVec<Real>&>(s1);
		const auto& x2 = dynamic_cast<const VarVec<Real>&>(s2);
		return euclideanDistance(x1.begin(), x1.end(), x2.begin());
	}

	void Continuous::evaluate_(SolBase& s, bool effective) {
		VarVec<Real>& x = dynamic_cast<Solution<>&>(s).variable();
		auto& obj = dynamic_cast<Solution<> &>(s).objective();
		auto& con = dynamic_cast<Solution<> &>(s).constraint();

		std::vector<Real> x_(x.begin(), x.end()); //for parallel running
		if (con.empty()) 
			evaluateObjective(x_.data(), obj);
		else  
			evaluateObjAndCon(x_.data(), obj, con);
	}

	int Continuous::updateEvalTag(SolBase &s, int id_alg, bool effective_eval) {
		if (boundaryViolated(s) || constraintViolated(s))
			return kInfeasible;
		else if (ID_ALG_VALID(id_alg) && GET_ALG(id_alg).solved())
#ifdef OFEC_DEMO
			return kNormalEval;
#else
			return kTerminate;
#endif // OFEC_DEMO
		else
			return kNormalEval;
	}

	bool Continuous::isOptimaObjGiven() const {
		return m_optima.isObjectiveGiven();
	}

	bool Continuous::isOptimaVarGiven() const {
		return m_optima.isVariableGiven();
	}

	Real Continuous::domainArea() {
		if (m_domain_update) {
			m_domain_area = 0;
			size_t limited_dim = 0;
			for (size_t j = 0; j < m_num_vars; ++j) {
				if (m_domain[j].limited) {
					m_domain_area += (m_domain[j].limit.second - m_domain[j].limit.first) * (m_domain[j].limit.second - m_domain[j].limit.first);
					limited_dim++;
				}
			}
			m_domain_area = sqrt(m_domain_area / limited_dim);
			m_domain_update = false;
		}
		return m_domain_area;
	}

	bool Continuous::same(const SolBase& s1, const SolBase& s2) const {
		return dynamic_cast<const Solution<> &>(s1).variable().vect() == dynamic_cast<const Solution<> &>(s2).variable().vect();
	}

}
