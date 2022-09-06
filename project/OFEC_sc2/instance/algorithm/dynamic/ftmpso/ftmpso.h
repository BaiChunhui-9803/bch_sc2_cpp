//Register FTMPSO "FTMPSO" ConOP,SOP,GOP,MMOP,DOP

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
// Created by PengMai at 12 July 2021
// Updated by PengMai at 07 September 2021 

/* ---------------------------------------------------------------------------------------
FTMPSO: [1] Yazdani D ,  Nasiri B ,  Sepas-Moghaddam A , et al.
A novel multi-swarm algorithm for optimization in dynamic environments based on particle swarm optimization[J].
Applied Soft Computing Journal, 2013, 13(4):2144-2158.
-----------------------------------------------------------------------------------------*/

#ifndef OFEC_FTMPSO_H
#define OFEC_FTMPSO_H
#include "ftmpso_subswarm.h"
#include "../../../../core/algorithm/multi_population.h"
#include "../../../../core/algorithm/algorithm.h"
#include "../../../problem/continuous/dynamic/uncertianty_continuous.h"
#include "../../../record/dynamic/rcr_vec_real_dynamic.h"
#include "../metrics_dynamic.h"
#ifdef OFEC_DEMO
#include "../dynamic_pso.h"
#endif

namespace ofec {
	class FTMPSO : public MetricsDynamicConOEA 
#ifdef OFEC_DEMO
		, public DynamicPSO
#endif
	{
	protected:
		void initialize_();
		void initializeFinder(int num);
		int initializeTracker(FTMPSOSwarm& parent, int num);
		void sortFinder(FTMPSOSwarm&);
		int getBestTracker();
		void updateFinderGBestMemory();
		int finderExclusion();
		void trackerExclusion();
		void removeExclusion();
		int isFinderInTracker();
		int activation();
		int trackerEvolve();
		int besttrackerTry();
		bool judgeHiberPop(FTMPSOSwarm&);
		void wakeupHiberPop();
		void relaunchTrackerSwarm(FTMPSOSwarm&);
		void informChange(int rf);

		void measureMultiPop(bool flag);
		void run_() override;

	public:
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer();
		std::vector<bool> getPopHiberState() const override;
#endif
	protected:
		ofec::MultiPopulation<FTMPSOSwarm> m_sub_pop;
		Real m_rconv; // threshold radius of conveged swarms
		Real m_rexcl; // radius of exlusion radius
		Real m_rs;	// radius of tracker swarm random x
		Real m_qs;	// radius of tracker swarm random v
		Real m_limit;	// hibernate limited vel for each particle
		int m_etry; // how many times the best tracker try to find better.
		Real m_rcloud;	//m_XR's domain
		Real m_cf;	//for m_Rcloud's update
		Real m_cfmin;	//for m_Rcloud's update
		std::vector<Solution<>> m_finderGbests;
		std::unique_ptr<Particle> m_xr;	// the particle generated from the best tracker Gbest
	};


}
#endif //!OFEC_FTMPSO_H
