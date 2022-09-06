#include "sade_pop.h"
#include "../../../../core/instance_manager.h"
#include <algorithm>

namespace ofec {
	PopSaDE::PopSaDE(size_t size_pop, int id_pro) : 
		PopDE(size_pop, id_pro), 
		m_num_strategy(4),
		m_LP(20),
		m_epsilon(0.01),
		mv_F(size_pop),
		mvv_CR(size_pop, std::vector<Real>(m_num_strategy)), 
		m_CRm(m_num_strategy, 0.5), 
		m_probability(m_num_strategy, 1. / m_num_strategy),
		m_strategy_selection(size_pop) 
	{
		for (size_t i = 0; i < m_probability.size(); ++i) {
			if (i > 0) m_probability[i] += m_probability[i - 1];
		}
	}

	int PopSaDE::evolve(int id_pro, int id_alg, int id_rnd) {
		std::vector<size_t> ridx;
		Real K;		
		updateCR(id_rnd);
		updateF(id_rnd);
		updateBest(id_pro);
		for (size_t i = 0; i < size(); ++i) {
			Real p = GET_RND(id_rnd).uniform.next() * m_probability[m_num_strategy - 1];
			m_strategy_selection[i] = lower_bound(m_probability.begin(), m_probability.end(), p) - m_probability.begin();
			switch (m_strategy_selection[i]) {
			case 0:		// DE/rand/1/bin
				select(i, 3, ridx, id_rnd);
				m_inds[i]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro);
				m_inds[i]->recombine(mvv_CR[i][m_strategy_selection[i]], RecombineDE::Binomial, id_rnd, id_pro);
				break;
			case 1:		// DE/rand-to-best/2/bin
				select(i, 4, ridx, id_rnd);
				m_inds[i]->mutate(m_F, m_inds[i].get(), m_best.front().get(), m_inds[i].get(), id_pro, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get());
				m_inds[i]->recombine(mvv_CR[i][m_strategy_selection[i]], RecombineDE::Binomial, id_rnd, id_pro);
				break;
			case 2:		// DE/rand/2/bin
				select(i, 5, ridx, id_rnd);
				m_inds[i]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro, m_inds[ridx[3]].get(), m_inds[ridx[4]].get());
				m_inds[i]->recombine(mvv_CR[i][m_strategy_selection[i]], RecombineDE::Binomial, id_rnd, id_pro);
				break;
			case 3:		// DE/current-to-rand/1
				K = GET_RND(id_rnd).uniform.next();
				select(i, 3, ridx, id_rnd);
				m_inds[i]->mutate(K, m_F, m_inds[i].get(), m_inds[ridx[0]].get(), m_inds[i].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro);
				break;
			}
		}
		int tag = kNormalEval;
		for (size_t i = 0; i < size(); ++i) {
			tag = m_inds[i]->select(id_pro, id_alg);
			if (tag != kNormalEval) 
				break;
		}
		if (tag == kNormalEval) {
			m_iter++;
		}
		updateMemory();
		return tag;
	}

	void PopSaDE::updateF(int id_rnd) {
		for (size_t i = 0; i < size(); i++) {
			mv_F[i] = GET_RND(id_rnd).normal.nextNonStd(0.5, 0.3);
		}
	}

	void PopSaDE::updateCR(int id_rnd) {
		if (m_iter >= m_LP) {
			for (size_t k = 0; k < m_num_strategy; ++k) {
				std::vector<Real> t;
				for (auto it = m_CRsuc.begin(); it != m_CRsuc.end(); ++it) {
					for (auto i : it->at(k)) t.push_back(i);
				}
				if (!t.empty()) {
					std::nth_element(t.begin(), t.begin() + t.size() / 2, t.end());
					m_CRm[k] = t[t.size() / 2];
				}
			}
		}
		for (size_t i = 0; i < size(); i++) {
			for (size_t k = 0; k < m_num_strategy; ++k) {
				do {
					mvv_CR[i][k] = GET_RND(id_rnd).normal.nextNonStd(m_CRm[k], 0.01);
				} while (mvv_CR[i][k] < 0 || mvv_CR[i][k]>1);
			}
		}
	}

	void PopSaDE::updateMemory() {
		std::vector < std::list<Real>> curmem(m_num_strategy);
		std::vector<int> curSuc(m_num_strategy), curFail(m_num_strategy);
		for (size_t i = 0; i < size(); i++) {
			if (m_inds[i]->isImproved()) {
				curmem[m_strategy_selection[i]].push_back(mvv_CR[i][m_strategy_selection[i]]);
				curSuc[m_strategy_selection[i]]++;
			}
			else {
				curFail[m_strategy_selection[i]]++;
			}
		}
		m_cnt_success.push_back(move(curSuc));
		m_CRsuc.push_back(move(curmem));
		m_cnt_fail.push_back(move(curFail));
		if (m_iter >= m_LP) {
			m_cnt_success.pop_front();
			m_CRsuc.pop_front();
			m_cnt_fail.pop_front();
			//update probability for all stategies
			for (size_t k = 0; k < m_num_strategy; ++k) {
				m_probability[k] = 0;
				int fail = 0;
				for (auto& j : m_cnt_success) m_probability[k] += j[k];
				for (auto& j : m_cnt_fail) fail += j[k];
				m_probability[k] = m_probability[k] / (m_probability[k] + fail) + m_epsilon;
				if (k > 0) m_probability[k] += m_probability[k - 1];
			}
		}
	}
}
