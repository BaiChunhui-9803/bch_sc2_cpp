//Register CEC2015_GOP_F10 "GOP_CEC2015_F10" GOP,ConOP,SOP

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************
*  Paper : Problem Definitions and Evaluation Criteria for the CEC2015
*  Competition on Learning-based Real-Parameter Single Objective
*  Optimization.
************************************************************************/

#ifndef OFEC_F10_GLOBAL_COMPOSITION2_H
#define OFEC_F10_GLOBAL_COMPOSITION2_H

#include "composition.h"
#include "hybrid.h"

namespace ofec {
	namespace CEC2015 {
		class F10_global_composition2 final : public composition_2015 {
		public:
			F10_global_composition2(const ParamMap &v);
			F10_global_composition2(const std::string &name, size_t size_var, size_t size_obj);
			~F10_global_composition2();
			Hybrid* get_hybrid(size_t num);
			void initialize();
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction();
			bool loadTranslation(const std::string &path);
			void setTranslation();
			void set_weight(std::vector<Real>& weight, const std::vector<Real>&x);
		protected:
			std::vector<Hybrid*> m_hybrid;
		};
	}
	using CEC2015_GOP_F10 = CEC2015::F10_global_composition2;
}
#endif // !OFEC_F10_GLOBAL_COMPOSITION2_H


