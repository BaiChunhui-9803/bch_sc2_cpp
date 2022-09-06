/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Long Xiao
* Email: 917003976@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://gitlab.com/OFEC/console for more information
*
*********************************************************************************/
// updated Feb 8, 2019 by Long Xiao


#ifndef OFEC_INIT_DVRP_POP_H
#define OFEC_INIT_DVRP_POP_H


#include "../../../../problem/realworld/DVRP/dynamic_vrp.h"
#include "../../../../../core/algorithm/individual.h"
#include "../../../../../core/algorithm/population.h"
#include "../../../DCO/DCMOEA.h"

#include "./AHC/AHC_TSP.h"
#include "./AHC/AHC_v2.h"

#if defined(WIN32)
#include <windows.h>
#endif

namespace ofec {
	class INIT_DVRP_POP 
	{
	public:
		using DVRP_ind = std::unique_ptr<DCMOEA_ind<individual<DVRP::routes, Real>>>;
		INIT_DVRP_POP() {}
		void ahc_tsp_init_pop(std::vector<DVRP_ind> &pop);
		void ahc_tsp_init_pop_(DVRP_ind &ind,int i);
		void randomly_init_pop(std::vector<DVRP_ind> &pop);
		void ahc_initialize_pop(std::vector<DVRP_ind> &pop);
	protected:
		void calcu_cus_order(std::vector<std::vector<size_t>> &m_cus_order);
		std::vector<size_t> tsp(std::vector<size_t>& member_index);
		void get_cost(size_t s, size_t e, Real &present_time);
	private:
		Real m_co_d = 0.3;
	};

}


#endif // OFEC_INIT_DVRP_POP_H

