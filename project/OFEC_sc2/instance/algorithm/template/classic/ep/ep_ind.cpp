#include "ep_ind.h"
#include "../../../../../core/problem/continuous/continuous.h"

namespace ofec {
	IndEP::IndEP(size_t num_objs, size_t num_cons, size_t num_vars) :
		Individual<>(num_objs, num_cons, num_vars), 
		m_eta(num_vars) {}

	void IndEP::initializeEta(int id_pro) {
		for (size_t i = 0; i < m_eta.size(); ++i)
			m_eta[i] = (GET_CONOP(id_pro).range(i).second - GET_CONOP(id_pro).range(i).first) / 10;
	}
}