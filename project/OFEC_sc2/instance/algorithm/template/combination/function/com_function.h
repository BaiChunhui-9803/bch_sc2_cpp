
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
* some general methods used in OFEC are defined in this file, including distance
* metrics, solution domination relationship,
*
*********************************************************************************/

#ifndef OFEC_COM_FINCTIONAL_H
#define OFEC_COM_FINCTIONAL_H


#include "../../../../../core/instance_manager.h"
#include "../../../../../core/definition.h"
#include "../../../../../utility/nondominated_sorting/fast_sort.h"
#include "../../../../../utility/nondominated_sorting/filter_sort.h"

namespace ofec {

	//template<typename TVecIndi>
	//inline void updateFitness(int id_pro,const TVecIndi& pop)
	//{
	//	
	//	if (GET_PRO(id_pro).hasTag(ProTag::MOP)) {
	//		std::vector<std::vector<Real>*> data(pop.size());
	//		for (size_t i = 0; i < pop.size(); ++i)
	//			data[i] = &(m_inds[i].objective());
	//		std::vector<int> rank;
	//		//NS::fast_sort<Real>(data, rank, global::ms_global->m_problem->optMode());
	//		if (m_inds.size() > 1e3) {
	//			NS::filter_sort_p<Real>(data, rank, GET_PRO(id_pro).optMode());
	//		}
	//		else {
	//			NS::filter_sort<Real>(data, rank, GET_PRO(id_pro).optMode());
	//		}
	//		sort(id_pro);
	//		for (auto& it : m_inds) {
	//			it->setFitness(it->rank());
	//		}
	//	}
	//	else {
	//		if (GET_PRO(id_pro).optMode()[0] == OptMode::Maximize) {
	//			for (auto& it : m_inds) {
	//				it->setFitness(1.0 / (it->objective(0) + 1e-5));
	//			}
	//		}
	//		else {
	//			for (auto& it : m_inds) {
	//				it->setFitness(it->objective(0));
	//			}
	//		}
	//	}
	//}

	//template<typename TInd>
	//inline void updateFitness(int id_pro, TInd& sol) const
	//{
	//	if (GET_PRO(id_pro).hasTag(ProTag::MOP)) {
	//		sol.setFitness(sol.rank());
	//	}
	//	else {
	//		if (GET_PRO(id_pro).optMode()[0] == OptMode::Maximize) {
	//			sol.setFitness(1.0 / (1e-5 + sol.objective(0)));
	//		}
	//		else {
	//			sol.setFitness(sol.objective(0));
	//		}
	//	}
	//}



}

#endif