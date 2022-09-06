//Register MOEA_F1 "MOEA_F1" MOP,ConOP
//Register MOEA_F2 "MOEA_F2" MOP,ConOP
//Register MOEA_F3 "MOEA_F3" MOP,ConOP
//Register MOEA_F4 "MOEA_F4" MOP,ConOP
//Register MOEA_F5 "MOEA_F5" MOP,ConOP
//Register MOEA_F6 "MOEA_F6" MOP,ConOP
//Register MOEA_F7 "MOEA_F7" MOP,ConOP
//Register MOEA_F8 "MOEA_F8" MOP,ConOP
//Register MOEA_F9 "MOEA_F9" MOP,ConOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 31 December 2014
// Modified: 29 Mar 2018 by Junchen Wang (wangjunchen@cug.edu.cn)

/*********************************************************************************************************************
  H. Li and Q. Zhang
  Comparison Between NSGA-II and MOEA/D on a Set of Multiobjective Optimization Problems with Complicated Pareto Sets
  Technical Report CES-476, Department of Computer Science, University of Essex, 2007
**********************************************************************************************************************/

#ifndef MOEA_FBASE_H
#define MOEA_FBASE_H

#include "../metrics_mop.h"

namespace ofec {
	class MOEA_F : public MetricsMOP {
	protected:
		void initialize_() override;
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;

		int getDtype() const { return m_dtype; }
		int getPtype() const { return m_ptype; }
		int getLtype() const { return m_ltype; }
		// control the PF shape
		void alphaFunction(Real alpha[], const Real *x, int dim, int type);
		// control the distance
		Real betaFunction(const std::vector<Real> & x, int type);
		// control the PS shape of 2-d instances
		Real paretoSetFunc2(const Real &x, const Real &t1, size_t dim, int type, int css);
		// control the PS shapes of 3-D instances
		Real paretoSetFunc3(const Real &x, const Real &t1, const Real &t2, int dim, int type);
		void LoadParetoFront();

		int m_dtype, m_ptype, m_ltype;
	};

	using MOEA_F1 = MOEA_F;
	using MOEA_F2 = MOEA_F;
	using MOEA_F3 = MOEA_F;
	using MOEA_F4 = MOEA_F;
	using MOEA_F5 = MOEA_F;
	using MOEA_F6 = MOEA_F;
	using MOEA_F7 = MOEA_F;
	using MOEA_F8 = MOEA_F;
	using MOEA_F9 = MOEA_F;
}
#endif //MOEA_FBASE_H