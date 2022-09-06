/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
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
// updated Jan 19, 2019 by Li Zhou

#ifndef OFEC_GL_POPULATION
#define OFEC_GL_POPULATION

#include "csiwdn_individual.h"
#include "../../../../core/algorithm/population.h"


namespace ofec {

	class GLPopulation : public Population<IndCSIWDN>
	{
	protected:
		std::vector<std::vector<std::pair<Real, size_t>>> m_node_data_obj;
		std::vector<std::vector<Real>> m_probability;
		long source_index;

	public:
		GLPopulation(size_t no,  int id_pro, size_t dim);
		GLPopulation & operator=(GLPopulation & pop) {
			Population::operator=(pop);	
			m_probability = pop.m_probability;
			m_node_data_obj = pop.m_node_data_obj;

			return *this;
		}
		GLPopulation & operator=(GLPopulation && pop) {
			Population::operator=(std::move(pop));
			m_probability = std::move(pop.m_probability);
			m_node_data_obj = std::move(pop.m_node_data_obj);
			
			return *this;
		}

		void mutate(int id_pro, int id_rnd);
		void updateProbability(int id_pro);
		
		void select(int id_pro, int id_alg, bool is_stable);
		void fillSolution(VarCSIWDN& indi, int id_pro);
		bool isFeasiblePopulation(int id_pro, const Real tar);

	};

}

#endif // !OFEC_GL_POPULATION

