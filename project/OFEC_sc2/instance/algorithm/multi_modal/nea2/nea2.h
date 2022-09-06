//Register NEA2 "NEA2" GOP,MMOP,ConOP,SOP

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
* NBC (Nearest-Better Clustering)
* Preuss, Mike.
* "Niching the CMA-ES via nearest-better clustering."
* Proceedings of the 12th annual conference companion on Genetic and evolutionary computation. 2010.
*-------------------------------------------------------------------------------
* Implemented by Junchen Wang (wangjunchen.chris@gmail.com) at 2020/9/23
*********************************************************************************/

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/multi_population.h"
#include "../../../../utility/clustering/nbc.h"
#include "../../global/cma_es/cmaes_pop.h"

namespace ofec {
	class NEA2 : public Algorithm {
	protected:
		MultiPopulation<PopCMAES> m_subpops;
		NBC<Individual<>> m_nbc;
		size_t m_pop_size;

		void initialize_() override;
		void run_() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:
		void record() override;

	private:
		void addSubpops();
		bool stopTolFun(PopCMAES& subpop);
	};
}

