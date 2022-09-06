#ifndef OFEC_JDE_POP_H
#define OFEC_JDE_POP_H

#include "../../template/classic/de/de_pop.h"

namespace ofec {
	class PopJDE final : public PopDE<> {
	public:
		PopJDE(size_t size_pop, int id_pro);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
	protected:
		void updateParams(int id_rnd);
	protected:
		Real m_t1 = 0, m_t2 = 0, m_Fl = 0, m_Fu = 0;
		std::vector<Real> mv_F, mv_CR;
	};
}


#endif // OFEC_JDE_POP_H
