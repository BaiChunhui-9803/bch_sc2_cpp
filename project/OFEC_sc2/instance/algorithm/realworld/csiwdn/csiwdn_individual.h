/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Apr 4, 2018 by Li Zhou

#ifndef OFEC_CSIWDN_INDIVIDUAL_H
#define OFEC_CSIWDN_INDIVIDUAL_H

#include "../../../../core/algorithm/individual.h"
#include "../../../problem/realworld/csiwdn/csiwdn_encoding.h"

#include <cmath>
#include <fstream>
#include <cstring>

namespace ofec {

	class IndCSIWDN : public Individual<VarCSIWDN>	{
	protected:
		SolType m_pv, m_pu;
	public:

		IndCSIWDN(size_t num_objs,size_t num_cons,size_t dim) : 
			Individual(num_objs, num_cons, dim),
			m_pv(num_objs, num_cons, dim), 
			m_pu(num_objs, num_cons, dim) {}

		IndCSIWDN(const IndCSIWDN& indi) : Individual(indi), m_pv(indi.m_pv), m_pu(indi.m_pu) {}
		IndCSIWDN(IndCSIWDN&& indi) : Individual(std::move(indi)), m_pv(std::move(indi.m_pv)), m_pu(std::move(indi.m_pu)) {}
		IndCSIWDN(Solution<VarCSIWDN>&& indi) :Individual(std::move(indi)), m_pv(indi), m_pu(indi) {};
		IndCSIWDN(const Solution<VarCSIWDN>& indi) :Individual(indi), m_pv(indi), m_pu(indi) {};

		IndCSIWDN& operator=(IndCSIWDN& indi);
		IndCSIWDN& operator=(IndCSIWDN&& indi);


		void mutateFirstPart(const std::vector<std::vector<Real>>& pro, int id_pro,int id_rnd);
		void mutateSecondPart(int id_pro,Real F, int jdx, const std::vector<std::vector<Real>> & pro,
			Solution<VarCSIWDN> *r1,
			Solution<VarCSIWDN> *r2,
			Solution<VarCSIWDN> *r3,
			Solution<VarCSIWDN> *r4 = 0,
			Solution<VarCSIWDN> *r5 = 0);
		SolType& mpu() {
			return m_pu;
		}
		SolType& mpv() {
			return m_pv;
		}

		void recombine(int id_pro, int id_rnd, Real CR);
		int select(int id_pro, int id_alg, bool is_stable);
		void coverFirstPart(VarCSIWDN& indi, int id_pro);
		void coverSecondPart(VarCSIWDN& indi, int id_pro);
		bool sameLocation(int id_pro, IndCSIWDN & indi);
		void initialize(int id, int id_pro, int id_rnd);
		
		SolType& trial();
	protected:
		void mutateMass(int id_pro, Real F, int jdx, Solution<VarCSIWDN> *r1,
			Solution<VarCSIWDN> *r2,
			Solution<VarCSIWDN> *r3,
			Solution<VarCSIWDN> *r4 = 0,
			Solution<VarCSIWDN> *r5 = 0);
		void mutateLocation(int id_pro, int id_rnd, const std::vector<std::vector<Real>>& pro);
		void mutateDuration(int id_pro, const std::vector<std::vector<Real>>& pro);
		int sourceIdx(int id_pro);


	public:
		Real m_distance_fitness;
		Real m_pu_distance_fitness;
		
	};


}

#endif // !OFEC_CSIWDN_INDIVIDUAL_H


