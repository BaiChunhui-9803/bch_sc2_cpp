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
// updated Jan 21, 2019 by Li Zhou

#ifndef OFEC_CC_FRAMEWORK_H
#define OFEC_CC_FRAMEWORK_H

#include <memory>
#include "../../../../core/algorithm/population.h"

namespace ofec {
	template <typename TPop1, typename TPop2, typename TInd>
	class CC_framework : public Population<TInd>   // just for epanet
	{
	public:
		/*friend class ADOPT<TPop1, TPop2, TInd>;
		friend class AMP<TPop1, TPop2, TInd>;
		friend class LRM_ADOPT<TPop1, TPop2, TInd>;*/

		enum class FillType { random, best };
		std::unique_ptr<TPop1> m_first_pop;
		std::unique_ptr<TPop2> m_second_pop;
		std::unique_ptr<TInd> m_best;
		std::vector<Real> m_history_best_obj;

		std::vector<std::vector<float>> m_history_mass;

	public:
		CC_framework() = default;
		CC_framework(size_t size,int id_pro) {
			m_first_pop = std::move(std::unique_ptr<TPop1>(new TPop1(size, id_pro, GET_CSIWDN(id_pro).numSource())));
			m_second_pop = std::move(std::unique_ptr<TPop2>(new TPop2(size, id_pro, GET_CSIWDN(id_pro).numSource())));
			size_t num_obj = GET_CSIWDN(id_pro).numObjectives();
			size_t num_cons = GET_CSIWDN(id_pro).numConstraints();
			m_best = std::move(std::unique_ptr<TInd>(new TInd(num_obj,num_cons, GET_CSIWDN(id_pro).numSource())));
			for (size_t i = 0; i < GET_CSIWDN(id_pro).numSource(); i++)
				m_best->variable().index(i) = -1;
		}
		virtual ~CC_framework() {

		}

		int evolve(int id_pro, int id_alg, int id_rnd) override;
		
		CC_framework & operator=(CC_framework & cc) {
			*m_first_pop = *cc.m_first_pop;
			*m_second_pop = *cc.m_second_pop;
			*m_best = *cc.m_best;
			m_history_best_obj = cc.m_history_best_obj;

			return *this;
		}

		CC_framework & operator=(CC_framework && cc) {
			*m_first_pop = std::move(*cc.m_first_pop);
			*m_second_pop = std::move(*cc.m_second_pop);
			*m_best = std::move(*cc.m_best);
			m_history_best_obj = std::move(cc.m_history_best_obj);

			return *this;
		}

		int fillEach(int id_rnd, int id_pro, int id_alg,FillType type);
		void updateBest(int id_pro,int id_alg);

		void initializeNewPop(int id_alg, int id_pro,int id_rnd, OFEC::VarCSIWDN& var_best);
		bool isStableObj();
		bool isStableMass();

		void recordHistoryMass(int id_pro);

		void clearHistory() {
			m_history_best_obj.clear();
		}

	};

	template<typename TPop1, typename TPop2, typename TInd>
	int CC_framework<TPop1, TPop2, TInd>::evolve(int id_pro, int id_alg, int id_rnd) {
		m_first_pop->updateProbability(id_pro);
		//m_second_pop->update_probability();
		m_second_pop->updateCR(id_pro,id_rnd);
		m_second_pop->updateF(id_pro, id_rnd);
		m_second_pop->updateProStrategy(id_pro);

		m_first_pop->mutate(id_pro, id_rnd);
		m_second_pop->mutate(id_pro, id_rnd);

		m_second_pop->recombine(id_pro, id_rnd);


		m_first_pop->select(id_pro, id_alg, false);
		m_second_pop->select(id_pro, id_alg, false);

		return fillEach(id_rnd,id_pro, id_alg, CC_framework<TPop1, TPop2, TInd>::FillType::best);

	}

	template <typename TPop1, typename TPop2, typename TInd>
	int CC_framework<TPop1, TPop2, TInd>::fillEach(int id_rnd, int id_pro, int id_alg,FillType type) {
		if (type == FillType::random) {
			size_t I = GET_RND(id_rnd).uniform.nextNonStd<size_t>(0, m_second_pop->size());
			m_first_pop->fillSolution((*m_second_pop)[I].variable(), id_pro);
			I = GET_RND(id_rnd).uniform.nextNonStd<size_t>(0, m_second_pop->size());
			m_second_pop->fillSolution((*m_first_pop)[I].variable(), id_pro);
		}
		else if (type == FillType::best) {
			
			m_first_pop->fillSolution(m_second_pop->best().front()->variable(), id_pro);
			m_second_pop->fillSolution(m_first_pop->best().front()->variable(), id_pro);
		}

		m_first_pop->evaluate(id_pro,id_rnd);
		m_second_pop->evaluate(id_pro, id_rnd);
		updateBest(id_pro,id_alg);
		m_history_best_obj.push_back(m_best->objective()[0]);
		return kNormalEval;
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void CC_framework<TPop1, TPop2, TInd>::updateBest(int id_pro,int id_alg) {
		size_t num_objs = GET_PRO(id_pro).numObjectives();
		size_t num_cons = GET_PRO(id_pro).numConstraints();
		size_t dim = GET_CSIWDN(id_pro).numberSource();
		TInd temp(num_objs,num_cons,dim);
		
		int z = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((*(m_first_pop->best().front())).variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep())) {
			z++;
		}
		temp = TInd(*(m_first_pop->best().front()));
		temp.variable().duration(z) = (*(m_first_pop->best().front())).variable().duration(z);
		temp.variable().startTime(z) = (*(m_first_pop->best().front())).variable().startTime(z);
		temp.variable().multiplier(z) = (*(m_first_pop->best().front())).variable().multiplier(z);

		temp.evaluate(id_pro, id_alg);

		if (m_best->variable().index(z) == -1) {
			*m_best = temp;
		}
		else {
			m_best->evaluate(id_pro, id_alg);
			if (temp.objective()[0] < m_best->objective()[0])
				*m_best = temp;
		}
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void CC_framework<TPop1, TPop2, TInd>::initializeNewPop(int id_alg,int id_pro,int id_rnd, OFEC::VarCSIWDN& var_best) {
		m_first_pop->initialize(id_pro, id_rnd);
		m_first_pop->evaluate(id_pro, id_alg);
		m_first_pop->updateBest(id_pro);
		m_second_pop->initialize(id_pro, id_rnd);
		m_second_pop->evaluate(id_pro, id_alg);
		m_second_pop->updateBest(id_pro);
		size_t z = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= (*m_first_pop)[0].variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()) {
			z++;
		}
		for (size_t i = 0; i < m_first_pop->size(); i++) {
			for (size_t j = 0; j < z; j++) {
				(*m_first_pop)[i].variable().getEpa(j) = var_best.getEpa(j);
				(*m_second_pop)[i].variable().getEpa(j) = var_best.getEpa(j);
				(*m_first_pop)[i].mpu().variable().getEpa(j) = var_best.getEpa(j);
				(*m_first_pop)[i].mpv().variable().getEpa(j) = var_best.getEpa(j);
				(*m_second_pop)[i].mpu().variable().getEpa(j) = var_best.getEpa(j);
				(*m_second_pop)[i].mpv().variable().getEpa(j) = var_best.getEpa(j);
			}
		}
		updateBest(id_pro,id_alg);
	}

	template <typename TPop1, typename TPop2, typename TInd>
	bool CC_framework<TPop1, TPop2, TInd>::isStableObj() {
		size_t standard = 5;
		if (m_history_best_obj.size() < standard) return false;
		int i = m_history_best_obj.size() - 1;
		int count = 0;
		while (m_history_best_obj[i] == m_history_best_obj[i - 1]) {
			++count;
			--i;
			if (i == 0) break;
		}
		return count >= standard - 1;
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void CC_framework<TPop1, TPop2, TInd>::recordHistoryMass(int id_pro) {
		int z = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((*(m_first_pop->best().front())).variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep())) {
			z++;
		}
		m_history_mass.push_back(m_best->variable().multiplier(z));
	}

	template <typename TPop1, typename TPop2, typename TInd>
	bool CC_framework<TPop1, TPop2, TInd>::isStableMass() {
		size_t standard = 3;
		if (m_history_mass.size() < standard) return false;
		int i = m_history_mass.size() - 1;
		int count = 0;
		while (m_history_mass[i] == m_history_mass[i - 1]) {
			++count;
			--i;
			if (i == 0) break;
		}
		return count >= standard - 1;
	}
}

#endif