/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 18 Mar 2021 by Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef OFEC_RGR_LOG_H
#define OFEC_RGR_LOG_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace ofec {
	class RegressLog : public Continuous {
	public:
		RegressLog(const std::vector<Real> &x, const std::vector<Real> &y);
		void updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const override;
	protected:
		void initialize_() override;
		void evaluateObjective(Real* x, std::vector<Real>& obj) override;
		std::vector<Real> m_x;
		std::vector<Real> m_y;
	};
}

#endif // !OFEC_RGR_LOG_H

