/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Operator adaptator of genetic learning algorithm
*
*********************************************************************************/

#ifndef OFEC_GL_ADAPTER_H
#define OFEC_GL_ADAPTER_H
#include <vector>
#include <memory>
#include "../../../../../core/definition.h"
#include <list>

namespace ofec {
	template<typename TInd>
	class AdaptorGL {
	protected:
	public:

		AdaptorGL(Real alpha) : m_alpha(alpha) {}
		AdaptorGL() = default;
		virtual ~AdaptorGL() {}
		virtual void updateProbability(int id_pro,int id_alg,
			std::vector<std::unique_ptr<TInd>>& pop, 
			const std::vector<Real>& weight, 
			const std::vector<int>* index = nullptr) = 0;
		virtual void updateProbability(int id_pro,int id_alg,
			std::vector<TInd>& pop, 
			const std::vector<Real>& weight, 
			const std::vector<int>* index = nullptr) {};
		virtual void createSolution(int id_pro,int id_alg, int id_rnd,
			std::vector<std::unique_ptr<TInd>>& pop, 
			std::vector<TInd>& offspring) = 0;
		virtual int updateSolution(int id_pro, int id_alg,
			std::vector<std::unique_ptr<TInd>>& pop, 
			std::vector<TInd>& offspring, int& num_improve) = 0;
		virtual void printInfo() {}
		Real getAlpha() const { return m_alpha; }
		void setAlpha(Real alpha) { m_alpha = alpha; }
	protected:
		Real m_alpha = 0.8;
	};
}

#endif //OFEC_GL_ADAPTER_H