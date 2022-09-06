#include "jde_pop.h"

namespace ofec {
	PopJDE::PopJDE(size_t size_pop, int id_pro) : 
		PopDE(size_pop, id_pro), 
		mv_F(size_pop), 
		mv_CR(size_pop),
		m_t1(0.1),
		m_t2(0.1),
		m_Fl(0.1),
		m_Fu(0.9) {}

	int PopJDE::evolve(int id_pro, int id_alg, int id_rnd) {
		updateParams(id_rnd);
		int tag = kNormalEval;
		std::vector<size_t> ridx(3);
		for (size_t i = 0; i < size(); ++i) {
			select(i, 3, ridx, id_rnd);
			m_inds[i]->mutate(mv_F[i], m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro);
			m_inds[i]->recombine(mv_CR[i], m_recombine_strategy, id_rnd, id_pro);
			tag = m_inds[i]->select(id_pro, id_alg);
			if (tag != kNormalEval)
				break;
		}
		if (tag == kNormalEval) {
			++m_iter;
		}
		return tag;
	}

	void PopJDE::updateParams(int id_rnd) {
		std::vector<Real> rand(4);
		if (m_iter > 0) {
			for (size_t i = 0; i < size(); ++i) {
				for (size_t j = 0; j < 4; ++j)
					rand[j] = GET_RND(id_rnd).uniform.next();
				if (rand[1] < m_t1) 
					mv_F[i] = m_Fl + rand[0] * m_Fu;
				if (rand[3] < m_t2)
					mv_CR[i] = rand[2];
			}
		}
		else {
			for (size_t i = 0; i < size(); ++i) {
				for (size_t j = 0; j < 4; j++)
					rand[j] = GET_RND(id_rnd).uniform.next();
				mv_F[i] = m_Fl + rand[0] * m_Fu;
				mv_CR[i] = rand[2];
			}
		}
	}
}


