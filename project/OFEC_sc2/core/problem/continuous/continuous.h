/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class Continuous defines continous optimization problems
*
*********************************************************************************/
#ifndef OFEC_CONTINUOUS_H
#define OFEC_CONTINUOUS_H

#include "../problem.h"
#include "../optima.h"
#include "../domain.h"
#include <vector>

namespace ofec {
#define GET_CONOP(id_pro) dynamic_cast<Continuous&>(GET_PRO(id_pro))

	class Continuous : virtual public Problem {
	public:
		Continuous() = default;
		virtual ~Continuous() = default;
		virtual void initialize_() override;
		void initSolution(SolBase& s, int id_rnd) const override;
		bool same(const SolBase& s1, const SolBase& s2) const override;
		Real variableDistance(const SolBase& s1, const SolBase& s2) const override;
		Real variableDistance(const VarBase& s1, const VarBase& s2) const override;
		bool boundaryViolated(const SolBase &s) const override; // boudary check only
		void validateSolution(SolBase& s, Validation mode, int id_rnd)const override;
		virtual void resizeVariable(size_t num_vars);
		virtual void resizeObjective(size_t num_objs) override;
		void updateParameters() override;
		virtual int updateEvalTag(SolBase &s, int id_alg, bool effective_eval) override;
		bool isOptimaObjGiven() const override;
		bool isOptimaVarGiven() const override;

		/* Read-only methods */
		virtual size_t numVariables() const override { return m_num_vars; }
		const std::pair<Real, Real>& range(size_t i) const { return m_domain.range(i).limit; }
		std::vector<std::pair<Real, Real>> boundary() const;
		const Optima<>& getOptima() const { return m_optima; }
		const Domain<Real>& domain() const { return m_domain; }
		const Domain<Real>& initialDomain() const { return m_initial_domain; }

		/* Write methods */
		Real domainArea();
		void setDomain(Real l, Real u);
		void setDomain(const std::vector<std::pair<Real, Real>>& r);
		void setInitialDomain(Real l, Real u);
		void setInitialDomain(const std::vector<std::pair<Real, Real>>& r);

	protected:
		void copy(const Problem&) override;
		void evaluate_(SolBase& s, bool effective) final override;
		virtual void evaluateObjective(Real* x, std::vector<Real>& obj) {}
		virtual void evaluateObjAndCon(Real* x, std::vector<Real>& obj, std::vector<Real>& con) {}

	protected:
		size_t m_num_vars;
		Domain<Real> m_domain;		// search domain
		bool m_domain_update;
		Real m_domain_area;
		Domain<Real> m_initial_domain; // domain for intialize population
		Optima<> m_optima;
	};

}
#endif // !OFEC_CONTINUOUS_H