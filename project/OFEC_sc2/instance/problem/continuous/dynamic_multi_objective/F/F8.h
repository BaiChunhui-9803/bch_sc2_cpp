//Register DMOP_F08 "DMOF_F08" DMOP,ConOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Qingshan Tan
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

* see https://github.com/Changhe160/OFEC for more information
*************************************************************************/

/************************************************************************
Aimin Zhou, Yaochu Jin, and Qingfu Zhang.
A population prediction strategy for evolutionary dynamic multiobjective optimization.
IEEE transactions on cybernetics, 44(1):40-53, 2013
************************************************************************/

// Created: 5 August 2019 by Qingshan Tan
// Last modified at 5 August 2019 by Qingshan Tan

#ifndef F8_H
#define F8_H


#include "../DMOPs.h"

namespace ofec {
    namespace DMOP {
        class F8 final : public DMOPs {
        public:
            F8(const ParamMap &v);

            F8(const std::string &name, size_t size_var);

            ~F8() {}

            void initialize();

            void generateAdLoadPF();

        private:
            void evaluateObjective(Real *x, std::vector<Real> &obj);
        };
    }
    using DMOP_F08 = DMOP::F8;
}

#endif //F8_H
