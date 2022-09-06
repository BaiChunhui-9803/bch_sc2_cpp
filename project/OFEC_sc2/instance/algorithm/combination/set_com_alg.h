//Register AS_TSP "AS_TSP" ComOP,TSP,SOP,MMOP,NoisyOP,DOP
//Register ACS_TSP "ACS_TSP" ComOP,TSP,SOP,MMOP,NoisyOP,DOP
//Register MMAS_TSP "MMAS_TSP" ComOP,TSP,SOP,MMOP,NoisyOP,DOP
//Register PACO_TSP "PACO_TSP" ComOP,TSP,SOP,MMOP,NoisyOP,DOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Yiya Diao
* Email: diaoyiyacug@163.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

Dorigo, M. (1996). "Ant system optimization by a colony of cooperating agents."
IEEE TRANSACTIONS ON SYSTEMS, MAN, AND CYBERNETICS.

*************************************************************************/
// Created: 7 Oct 2014
// Last modified:
// updated: by Yiya Diao in July  2021

#ifndef OFEC_SET_COM_ALG_H
#define OFEC_SET_COM_ALG_H

#include "aco/aco.h"
#include "aco/as/as.h"
#include "aco/acs/acs.h"
#include "aco/mmas/mmas.h"
#include "aco/paco/paco.h"
#include "sequence/tsp/tsp_interpreter.h"

namespace ofec {
	using AS_TSP = ACO<AS<InterpreterTSP>>;
	using ACS_TSP = ACO<ACS<InterpreterTSP>>;
	using MMAS_TSP = ACO<MMAS<InterpreterTSP>>;
	using PACO_TSP = ACO<PACO<InterpreterTSP>>;
}

#endif