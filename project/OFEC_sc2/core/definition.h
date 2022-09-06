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
* macros used in OFEC
*
*
*********************************************************************************/

#ifndef OFEC_DEFINITION_H
#define OFEC_DEFINITION_H

#include <cmath>

namespace ofec {
#define OFEC_PI std::acos(-1.0)
#define OFEC_E std::exp(1.0)

#ifdef USING_FLOAT                     
	using Real = float;						// set precision type to float
#else
	using Real = double;				  // set precision type to double
#endif

	enum class Dominance { kEqual, kDominant, kDominated, kNonDominated, kNonComparable };
	enum class OptMode { kMinimize, kMaximize };
	enum class Violation { kConstraint, kBoundary, kNone };
	enum class Constraint{ kInequality, kEquality };
	enum class Validation { kIgnore, kReinitialize, kRemap, kSetToBound };

	/*using the binary coding to record the evaluation tag:
	   cur_tag & kTerminate == true, means that cur_tag has kTerminate tag;
	   cur_tag |= kTerminate, insert the kTerminate tag into cur_tag*/
	enum EvalTag {
		kNormalEval = 1 << 1,
		kTerminate = 1 << 2,
		kInfeasible = 1 << 3,
		kChangeNextEval = 1 << 4,
		kChangeCurEval = 1 << 5,
		kChangeObjectiveMemory = 1 << 6,
		kChangeVariableMemory = 1 << 7
	};

	//SOP: single objective problem
	//MOP: multi-objective problem
	//DOP: dynamic optimization problem
	//MMOP: multi-modal optimization problem
	//GOP: global optimization problem
	//ROOT: robust optimzation problem
	//ConOP: Continuous optimization problem
	//ComOP: combinatorial optimization problem
	//TSP: travelling salesman problem
	//COP: constraint optimization problem
	//VRP: vehicle routing problem
	//TTP: timetabling problem
	//JSP: job shop problem
	//KOP: knapsack optimization problem
	//SAT: boolean satisfiability problem
	//ONEMAX: one max problem
	//QAP: quadratic assignment problem
	//MKP: multi-dimensional knapsack problem
	//EOP: expensive optimization problem
	//LSOP: large scale optimization problem
	//CSIWDN: contaminant source identification for water distribution network 
	//SP: selection problem
	//APP: airport planning problem
	//NoisyOP: noisy optimization problem
	enum class ProTag {NullTag,
		kSOP, kMOP, kDOP, kDMOP, kMMOP, kGOP, kROOT, kConOP, kComOP, kTSP, kCOP, kVRP, kTTP, kJSP,
		kKOP, kSAT, kOneMax, kQAP, kMKP, kEOP, kLSOP, kCSIWDN, kDVRP, kSP, kAPP, kNoisyOP
	};
}
#endif // !OFEC_DEFINITION_H