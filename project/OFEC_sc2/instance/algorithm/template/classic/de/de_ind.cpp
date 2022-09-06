#include "de_ind.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	IndDE::IndDE(size_t num_objs, size_t num_cons, size_t num_vars) :
		Individual<>(num_objs, num_cons, num_vars),
		m_pv(num_objs, num_cons, num_vars),
		m_pu(num_objs, num_cons, num_vars) {}

	IndDE::IndDE(const Solution<>& sol) :
		Individual<>(sol),
		m_pu(sol),
		m_pv(sol) {}

	IndDE& IndDE::operator=(const Solution<>& other) {
		Solution<>::operator=(other);
		return *this;
	}

	void IndDE::recombine(Real CR, RecombineDE rs, int id_rnd, int id_pro) {
		size_t dim = variable().size();
		if (rs == RecombineDE::Binomial) {
			size_t I = GET_RND(id_rnd).uniform.nextNonStd<size_t>(0, dim);
			for (size_t i = 0; i < dim; ++i) {
				Real p = GET_RND(id_rnd).uniform.next();
				if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
				else m_pu.variable()[i] = variable()[i];
			}
		}
		else if (rs == RecombineDE::exponential) {
			int n = GET_RND(id_rnd).uniform.nextNonStd<int>(0, dim);
			size_t L = 0;
			do {
				L = L + 1;
			} while (GET_RND(id_rnd).uniform.next() < CR && L < dim);
			for (size_t i = 0; i < dim; i++) {
				if (i < L)
					m_pu.variable()[(n + i) % dim] = m_pv.variable()[(n + i) % dim];
				else
					m_pu.variable()[(n + i) % dim] = variable()[(n + i) % dim];
			}
		}
		GET_CONOP(id_pro).validateSolution(m_pu, Validation::kSetToBound, id_rnd);
	}

	void IndDE::mutate(Real F,
		const Solution<> *s1,
		const Solution<> *s2,
		const Solution<> *s3,
		int id_pro,
		const Solution<> *s4,
		const Solution<> *s5,
		const Solution<>* s6,
		const Solution<>* s7) 
	{
		Real l, u;
		for (size_t i = 0; i < m_pv.variable().size(); ++i) {
			l = GET_CONOP(id_pro).range(i).first;
			u = GET_CONOP(id_pro).range(i).second;
			m_pv.variable()[i] = (s1->variable()[i]) + F * ((s2->variable()[i]) - (s3->variable()[i]));
			if (s4 && s5) m_pv.variable()[i] += F * ((s4->variable()[i]) - (s5->variable()[i]));
			if (s6 && s7) m_pv.variable()[i] += F * ((s6->variable()[i]) - (s7->variable()[i]));
			if ((m_pv.variable()[i]) > u) {
				m_pv.variable()[i] = ((s1->variable()[i]) + u) / 2;
			}
			else if ((m_pv.variable()[i]) < l) {
				m_pv.variable()[i] = ((s1->variable()[i]) + l) / 2;
			}
		}
	}

	void IndDE::mutate(Real F,
		const std::vector<int>& var,
		const Solution<> *s1,
		const Solution<> *s2,
		const Solution<> *s3,
		int id_pro,		 
		const Solution<> *s4,
		const Solution<> *s5,
		const Solution<>* s6,
		const Solution<>* s7)
	{
		Real l, u;
		for (auto i : var) {
			l = GET_CONOP(id_pro).range(i).first;
			u = GET_CONOP(id_pro).range(i).second;
			m_pv.variable()[i] = (s1->variable()[i]) + F * ((s2->variable()[i]) - (s3->variable()[i]));
			if (s4 && s5) m_pv.variable()[i] += F * ((s4->variable()[i]) - (s5->variable()[i]));
			if (s6 && s7) m_pv.variable()[i] += F * ((s6->variable()[i]) - (s7->variable()[i]));
			if ((m_pv.variable()[i]) > u) {
				m_pv.variable()[i] = ((s1->variable()[i]) + u) / 2;
			}
			else if ((m_pv.variable()[i]) < l) {
				m_pv.variable()[i] = ((s1->variable()[i]) + l) / 2;
			}
		}
	}

	void IndDE::mutate(Real K, Real F, 
		const Solution<>* s1, 
		const Solution<>* s2, 
		const Solution<>* s3, 
		const Solution<>* s4, 
		const Solution<>* s5, 
		int id_pro)
	{
		Real l, u;
		for (size_t i = 0; i < m_pv.variable().size(); ++i) {
			l = GET_CONOP(id_pro).range(i).first;
			u = GET_CONOP(id_pro).range(i).second;
			m_pv.variable()[i] = (s1->variable()[i]) + K * ((s2->variable()[i]) - (s3->variable()[i]))+ F * ((s4->variable()[i]) - (s5->variable()[i]));
			if ((m_pv.variable()[i]) > u) {
				m_pv.variable()[i] = ((s1->variable()[i]) + u) / 2;
			}
			else if ((m_pv.variable()[i]) < l) {
				m_pv.variable()[i] = ((s1->variable()[i]) + l) / 2;
			}
		}
	}

	void IndDE::recombine(Real CR, const std::vector<int>& var, int I, int id_rnd) {
		for (auto i : var) {
			Real p = GET_RND(id_rnd).uniform.next();
			if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
			else m_pu.variable()[i] = variable()[i];
		}
	}

	int IndDE::select(int id_pro, int id_alg) {
		int tag = m_pu.evaluate(id_pro, id_alg);
		if (m_pu.dominate(*this, GET_PRO(id_pro).optMode())) {
			variable() = m_pu.variable();
			m_obj = m_pu.objective();
			m_con = m_pu.constraint();
			m_improved = true;
		}
		else {
			m_improved = false;
		}
		return tag;
	}
}
