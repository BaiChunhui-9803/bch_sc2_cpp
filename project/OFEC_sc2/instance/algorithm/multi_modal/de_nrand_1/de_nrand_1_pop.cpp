#include "de_nrand_1_pop.h"

namespace ofec {
	PopNRand1DE::PopNRand1DE(size_t size_pop, int id_pro) : 
		PopDE(size_pop, id_pro) {}

	int PopNRand1DE::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		std::vector<size_t> ridx;
		size_t nearest;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			select(i, 2, ridx, id_rnd);
			auto knn = nearestNeighbour(i, id_pro, 3);
			nearest = GET_RND(id_rnd).uniform.nextElem(knn.begin(), knn.end())->second;
			//nearest = nearestNeighbour(i, id_pro, 5).begin()->second;
			m_inds[i]->mutate(m_F, m_inds[nearest].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), id_pro);
			recombine(i, id_rnd, id_pro);
			tag = m_inds[i]->select(id_pro, id_alg);
			if (tag == EvalTag::kTerminate) break;
		}
		if (tag == kNormalEval)
			m_iter++;
		return tag;
	}
}

