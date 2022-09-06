#include "con_sa.h"
#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	void ConSA::initSol() {
		size_t num_objs = GET_PRO(m_id_pro).numObjectives();
		size_t num_cons = GET_PRO(m_id_pro).numConstraints();
		size_t num_vars = GET_CONOP(m_id_pro).numVariables();
		m_s.reset(new Solution<>(num_objs, num_cons, num_vars));
		m_s_new.reset(new Solution<>(num_objs, num_cons, num_vars));
		m_s->initialize(m_id_pro, m_id_rnd);
		m_s->evaluate(m_id_pro, m_id_alg);
	}

	void ConSA::neighbour(const Solution<>& s, Solution<>& s_new) {
		for (size_t i = 0; i < s.variable().size(); i++) {
			Real variance = (GET_CONOP(m_id_pro).range(i).second - GET_CONOP(m_id_pro).range(i).first) / 100;
			do {
				s_new.variable()[i] = GET_RND(m_id_rnd).normal.nextNonStd(s.variable()[i], variance);
			} while (s_new.variable()[i] < GET_CONOP(m_id_pro).range(i).first || s.variable()[i] > GET_CONOP(m_id_pro).range(i).second);
		}
	}
}