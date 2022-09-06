#ifndef OFEC_SADE_POP_H
#define OFEC_SADE_POP_H

#include <list>
#include "../../template/classic/de/de_pop.h"

namespace ofec {
	class PopSaDE final : public PopDE<> {
	public:
		PopSaDE(size_t size_pop, int id_pro);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
#ifdef OFEC_DEMO
		const std::vector<Real>& ratioStrategy() const { return m_probability; }
#endif

	protected:
		void updateF(int id_rnd);
		void updateCR(int id_rnd);
		void updateMemory();

	protected:
		const size_t m_num_strategy;
		size_t m_LP;
		Real m_epsilon;
		Violation m_vio_mode = Violation::kBoundary;

		// 0: DE/rand/1/bin
		// 1: DE/rand-to-best/2/bin
		// 2: DE/rand/2/bin
		// 3: DE/current-to-rand/1
		std::vector<int> m_strategy_selection;
		std::vector<Real> mv_F;
		std::vector<std::vector<Real>> mvv_CR;
		std::list<std::vector<std::list<Real>>> m_CRsuc;
		std::vector<Real> m_CRm, m_probability;
		std::list<std::vector<int>> m_cnt_success, m_cnt_fail;
	};
}
#endif // OFEC_SADE_POP_H
