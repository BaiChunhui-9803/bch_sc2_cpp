#ifndef OFEC_JADE_POP_H
#define OFEC_JADE_POP_H

#include "../../template/classic/de/de_pop.h"

namespace ofec {
	class PopJADE : public PopDE<> {
	public:
		PopJADE(size_t size_pop, int id_pro);
		int evolve(int id_pro, int id_alg, int id_rnd) override;

	protected:
		void selectTrial(size_t base, int id_rnd);
		void updateF(int id_rnd);
		void updateCR(int id_rnd);

	protected:
		///////////////////////algorithm parameters/////////////////////////////
		Real m_p;
		Real m_c;
		int m_archive_flag;
		std::vector<Solution<>> m_archive;
		std::vector<Solution<>*> m_candi;
		std::vector<Real> m_pcent_best;
		std::vector<int> m_pcent_best_index;
		std::vector<Real> mv_F, mv_CR;
		///////////////////////////////////////////////////////////////////////////
	};
}
#endif // OFEC_JADE_POP_H
