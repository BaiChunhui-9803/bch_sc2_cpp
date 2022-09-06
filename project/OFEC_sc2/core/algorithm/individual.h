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
* class Individual is an abstract class for all types of individuals of an evolutionary
* computation algorithm
*
*********************************************************************************/

#ifndef OFEC_INDIVIDUAL_H
#define OFEC_INDIVIDUAL_H

#include "../problem/solution.h"

namespace ofec {
	template<typename TVar = VarVec<Real>>
		class Individual : public Solution<TVar> {
		public:
			using SolType = Solution<TVar>;
			Individual() = default;
			virtual ~Individual() {}
			template<typename ... Args>
			Individual(size_t num_obj, size_t num_con, Args&& ... args) : 
				SolType(num_obj, num_con, std::forward<Args>(args)...) {}
			Individual(SolType &&s) : SolType(std::move(s)) {}
			Individual(const SolType &s) : SolType(s) {}

			virtual void initialize(int id, int id_pro, int id_rnd) {
				m_id = id;
				SolType::initialize(id_pro, id_rnd);
			}

			void setId(int id)noexcept {
				m_id = id;
			}
			void setFitness(Real value) noexcept {
				m_fitness = value;
			}
			void setType(int type)noexcept {
				m_type = type;
			}
			void setRank(int value) noexcept {
				m_ranking = value;
			}
			void setImproved(bool value) noexcept {
				m_improved = value;
			}
			void setActive(bool value) noexcept {
				m_active = value;
			}

			int id()const noexcept {
				return m_id;
			}
			Real fitness()const noexcept {
				return m_fitness;
			}
			int type()const noexcept {
				return m_type;
			}
			int rank() const noexcept {
				return m_ranking;
			}
			bool isImproved()const noexcept {
				return m_improved;
			}
			bool isActive()const noexcept {
				return m_active;
			}
			Individual(const Individual& rhs) = default;
			Individual(Individual&& rhs) = default;
			Individual& operator=(const Individual &rhs) = default;
			Individual& operator=(Individual &&rhs) = default;
			virtual const SolType& phenotype() const { return SolType::solut(); }
			void reset() override {
				SolBase::reset();
				m_fitness = 0;
				m_id = -1;
				m_ranking = -1;
				m_type = 0;
				m_improved = false;
				m_active = true;
			}
		protected:
			Real m_fitness = 0;
			int m_id, m_ranking = -1, m_type;
			bool m_improved = false, m_active = true;
	};
}
#endif // !OFEC_INDIVIDUAL_H
