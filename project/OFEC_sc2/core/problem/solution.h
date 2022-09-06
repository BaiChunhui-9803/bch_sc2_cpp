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
* class solution is designed for any kind of user-defined solution representation.
* It takes a variable encoding and an objective value type as parameters to generate
* a solution.
*
*********************************************************************************/

#ifndef OFEC_SOLUTION_H
#define OFEC_SOLUTION_H

#include <utility>
#include "../instance_manager.h"

namespace  ofec {

	template<typename TVar = VarVec<Real>>
	class Solution : public SolBase {
	public:
		using VarEnc = TVar;
		virtual ~Solution() {}
		Solution() = default;
		template<typename ... Args>
		Solution(size_t num_objs, size_t num_cons, Args&& ... args) : SolBase(num_objs, num_cons) {
			m_var.reset(new TVar(std::forward<Args>(args)...));
		}

		Solution(const Solution &sol) : SolBase(sol) {
			m_var.reset(new TVar(dynamic_cast<const TVar &>(*sol.m_var)));
		}

		Solution(Solution &&sol) : SolBase(std::move(sol)) {
			m_var.reset(sol.m_var.release());
		}

		Solution& operator=(const Solution &rhs) { 
			if (this != &rhs) {
				SolBase::operator=(rhs);
				if (m_var)
				{
					auto& cur_var(dynamic_cast<TVar&>((*m_var)));
					cur_var = dynamic_cast<const TVar&>(*rhs.m_var);
				}
				else
					m_var.reset(new TVar(dynamic_cast<const TVar &>(*rhs.m_var)));
			}
			return *this; 
		}

		Solution& operator=(Solution &&rhs) { 
			if (this != &rhs) {
				SolBase::operator=(std::move(rhs));
				m_var.reset(rhs.m_var.release());
			}
			return *this;
		}

		Solution(const SolBase &sol) : Solution(dynamic_cast<const Solution&>(sol)) {}

		Solution(SolBase &&sol) : Solution(dynamic_cast<Solution&&>(sol)) {}

		const Solution& solut() const noexcept { return *this; }
		Solution& solut() noexcept { return *this; }
		const TVar& variable() const noexcept { return dynamic_cast<const TVar&>(*m_var); }
		TVar& variable() noexcept { return dynamic_cast<TVar&>(*m_var); }

		Real variableDistance(const SolBase &x, int id_pro) const override {
			return GET_PRO(id_pro).variableDistance(*this, x);
		}

		Real variableDistance(const VarBase &x, int id_pro) const override {
			return GET_PRO(id_pro).variableDistance(*m_var, x);
		}
	};
}

#endif // !OFEC_SOLUTION_H
