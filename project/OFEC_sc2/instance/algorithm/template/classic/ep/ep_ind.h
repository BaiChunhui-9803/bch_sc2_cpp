/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created by Junchen Wang on Oct. 28, 2018.

#ifndef OFEC_EP_IND_H
#define OFEC_EP_IND_H

#include "../../../../../core/algorithm/individual.h"

namespace ofec {
	class IndEP : public Individual<> {
	protected:
		std::vector<Real> m_eta;
	public:
		IndEP(size_t num_objs, size_t num_cons, size_t num_vars);
		virtual void initializeEta(int id_pro);
		std::vector<Real>& eta() { return m_eta; }
		const std::vector<Real>& eta()const { return m_eta; }
	};
}


#endif // !OFEC_EP_IND_H

