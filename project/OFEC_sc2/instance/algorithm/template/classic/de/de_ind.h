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
*-------------------------------------------------------------------------------
* class DEindividual is a class for differential evolution of individuals of an
* evolutionary computation algorithm.
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou

#ifndef DE_INDIVIDUAL_H
#define DE_INDIVIDUAL_H

#include "../../../../../core/algorithm/individual.h"

namespace ofec {
	enum class RecombineDE { Binomial, exponential };
	class IndDE : public ofec::Individual<> {
	protected:
		Solution<> m_pv;    // donor vector
		Solution<> m_pu;    // trial vector
	public:
		virtual ~IndDE() {}
		IndDE(size_t num_objs, size_t num_cons, size_t num_vars);
		IndDE(const IndDE&) = default;
		IndDE(IndDE&& p) noexcept = default;
		IndDE(const Solution<>& sol);
		IndDE& operator=(const IndDE& other) = default;
		IndDE& operator=(IndDE&& other) noexcept = default;
		IndDE& operator=(const Solution<>& other);
		void mutate(Real F, 
			const Solution<>* s1,
			const Solution<>* s2,
			const Solution<>* s3,
			int id_pro,
			const Solution<>* s4 = nullptr,
			const Solution<>* s5 = nullptr,
			const Solution<>* s6 = nullptr,
			const Solution<>* s7 = nullptr);
		virtual void recombine(Real CR, RecombineDE rs, int id_rnd, int id_pro);
		virtual int select(int id_pro, int id_alg);
		Solution<> &trial() { return m_pu; }
		const Solution& trial() const { return m_pu; }
		Solution<>& donor() { return m_pv; }
		const Solution &donor() const { return m_pv; }
		void recombine(Real CR, const std::vector<int>& var, int I, int id_rnd);
		void mutate(Real F, 
			const std::vector<int>& var, 
			const Solution<>* s1,
			const Solution<>* s2,
			const Solution<>* s3,
			int id_pro,
			const Solution<>* s4 = nullptr,
			const Solution<>* s5 = nullptr,
			const Solution<>* s6 = nullptr,
			const Solution<>* s7 = nullptr);
		void mutate(Real K, Real F,
			const Solution<>* s1,
			const Solution<>* s2,
			const Solution<>* s3,
			const Solution<>* s4,
			const Solution<>* s5,
			int id_pro);
	};
}
#endif // !DE_INDIVIDUAL_H
