#include "hgspc_de.h"
#include "../../../../core/instance_manager.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/problem/solution.h"
#include "../../template/framework/hgea/continuous/hgspc_adaptor.h"

namespace ofec {
	void HGSPC_DE::initialize_() {
		HGSPC::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_F = v.count("scaling factor") > 0 ? std::get<Real>(v.at("scaling factor")) : 0.5;
		m_CR = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.6;
	}

	void HGSPC_DE::reproduceSolution(size_t id_ind) {
		std::vector<int> candidate;
		for (size_t i = 0; i < m_parents[id_ind].size(); ++i) {
			if (id_ind != i) candidate.push_back(i);
		}
		std::vector<const Solution<>*> s(4);
		for (size_t i = 0; i < 4; ++i) {
			size_t idx = GET_RND(m_id_rnd).uniform.nextNonStd<size_t>(0, candidate.size() - i);
			s[i] = dynamic_cast<const Solution<>*>(m_parents[id_ind][candidate[idx]]);
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}

		std::vector<Real> pv(GET_PRO(m_id_pro).numVariables());
		Real l, u;
		for (size_t j = 0; j < pv.size(); ++j) {
			l = GET_CONOP(m_id_pro).range(j).first;
			u = GET_CONOP(m_id_pro).range(j).second;
			pv[j] = s[1]->variable()[j] + m_F * (s[2]->variable()[j] - s[3]->variable()[j]);
			if (pv[j] > u) {
				pv[j] = (s[1]->variable()[j] + u) / 2;
			}
			else if ((pv[j]) < l) {
				pv[j] = (s[1]->variable()[j] + l) / 2;
			}
		}

		auto &pu = dynamic_cast<Solution<>&>(*m_offspring[id_ind]);
		size_t I = GET_RND(m_id_rnd).uniform.nextNonStd<size_t>(0, pu.variable().size());
		for (size_t i = 0; i < pu.variable().size(); ++i) {
			Real p = GET_RND(m_id_rnd).uniform.next();
			if (p <= m_CR || i == I)     pu.variable()[i] = pv[i];
			else pu.variable()[i] = s[0]->variable()[i];
		}

		GET_CONOP(m_id_pro).validateSolution(pu, Validation::kSetToBound, m_id_rnd);
	}
}