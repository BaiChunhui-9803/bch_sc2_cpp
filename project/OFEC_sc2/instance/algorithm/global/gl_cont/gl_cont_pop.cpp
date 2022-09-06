#include "gl_cont_pop.h"
#include "gl_adaptor_cont.h"
#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	void PopContGL::resize(size_t size_pop, int id_pro) {
		PopGL<Individual<>>::resize(size_pop, id_pro, GET_CONOP(id_pro).numVariables());
	}

	void PopContGL::initialize(int id_pro, int id_rnd) {
		m_adaptor.reset(new AdaptorContGL(m_alpha, GET_CONOP(id_pro).numVariables(), m_inds.size()));
		Population<Individual<>>::initialize(id_pro, id_rnd);
		initializeMemory(id_pro);
		initializeCurpop();
	}

	void PopContGL::initializeCurpop() {
		for (int i = 0; i < this->size(); i++) {
			m_offspring.push_back(*this->m_inds[i]);
		}
	}

	int PopContGL::evolve(int id_pro, int id_alg, int id_rnd) {
		dynamic_cast<AdaptorContGL *>(m_adaptor.get())->updateStep(id_pro, m_inds);
		m_adaptor->createSolution(id_pro, id_rnd, m_inds, m_offspring);
		int rf = update(id_pro, id_alg);
		for (auto &i : m_inds)
			if (i->isImproved())
				updateBest(*i, id_pro);
		updateMemory(id_pro);
		m_iter++;
		return rf;
	}
}
