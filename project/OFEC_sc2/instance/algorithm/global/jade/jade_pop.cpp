#include "jade_pop.h"

namespace ofec {
	PopJADE::PopJADE(size_t size_pop, int id_pro) : 
		PopDE(size_pop, id_pro),
		m_candi(3, nullptr), 
		m_pcent_best(size_pop), 
		m_pcent_best_index(size_pop), 
		mv_F(size_pop), 
		mv_CR(size_pop),
		m_archive_flag(1),
		m_p(0.2),
		m_c(0.1) {}

	void PopJADE::selectTrial(size_t base, int id_rnd) {
		std::vector<int> candidate;
		for (size_t i = 0; i < size(); i++) {
			if (base != i) candidate.push_back(i);
		}
		size_t idx;
		do {
			idx = m_pcent_best_index[GET_RND(id_rnd).uniform.nextNonStd<size_t>(0, size() * m_p)];
		} while (idx == base);

		m_candi[0] = m_inds[idx].get();

		const auto it = std::find(candidate.begin(), candidate.end(), idx);
		candidate.erase(it);

		idx = GET_RND(id_rnd).uniform.nextNonStd<int>(0, (int)candidate.size());
		m_candi[1] = m_inds[candidate[idx]].get();
		candidate.erase(candidate.begin() + idx);

		idx = GET_RND(id_rnd).uniform.nextNonStd<int>(0, (int)(candidate.size() + m_archive.size()));

		if (idx >= candidate.size()) {
			m_candi[2] = &m_archive[idx - candidate.size()];
		}
		else {
			m_candi[2] = m_inds[candidate[idx]].get();
		}

	}

	int PopJADE::evolve(int id_pro, int id_alg, int id_rnd) {
		updateCR(id_rnd);
		updateF(id_rnd);
		int tag = kNormalEval;
		for (size_t i = 0; i < size(); ++i)
			m_pcent_best[i] = m_inds[i]->objective()[0];	
		mergeSort(m_pcent_best, (int)size(), m_pcent_best_index, GET_PRO(id_pro).optMode(0) == OptMode::kMinimize);
		std::vector<int> candidate(3);
		for (size_t i = 0; i < size(); ++i) {
			selectTrial(i, id_rnd);
			m_inds[i]->mutate(mv_F[i], m_inds[i].get(), m_candi[0], m_inds[i].get(), id_pro, m_candi[1], m_candi[2]);
			m_inds[i]->recombine(mv_CR[i], m_recombine_strategy, id_rnd, id_pro);
		}
		for (size_t i = 0; i < size(); i++) {
			tag = m_inds[i]->select(id_pro, id_alg);
			if (!m_inds[i]->isImproved())
				m_archive.push_back(m_inds[i]->solut());
			while (m_archive.size() > size()) {
				int ridx = GET_RND(id_rnd).uniform.nextNonStd<int>(0, m_archive.size());
				m_archive.erase(m_archive.begin() + ridx);
			}
			if (tag != kNormalEval) 
				break;
		}
		if (tag == kNormalEval) {
			m_iter++;
		}
		return tag;
	}

	void PopJADE::updateF(int id_rnd) {
		Real mu = 0.5;
		if (m_iter > 0) {
			Real mean = 0, mean2 = 0;
			int cnt = 0;
			for (size_t i = 0; i < size(); i++) {
				if (m_inds[i]->isImproved()) {
					cnt++;
					mean += mv_F[i];
					mean2 += mv_F[i] * mv_F[i];
				}
			}
			if (cnt > 0) {
				mean = mean2 / mean;
			}

			mu = (1 - m_c) * mu + m_c * mean;
		}
		for (size_t i = 0; i < size(); i++) {
			do {
				mv_F[i] = GET_RND(id_rnd).cauchy.nextNonStd(mu, 0.01);
			} while (mv_F[i] <= 0);

			if (mv_F[i] > 1) mv_F[i] = 1;
		}
	}

	void PopJADE::updateCR(int id_rnd) {
		Real mu = 0.5;
		if (m_iter > 0) {
			Real mean = 0;
			int cnt = 0;
			for (size_t i = 0; i < size(); i++) {
				if (m_inds[i]->isImproved()) {
					cnt++;
					mean += mv_CR[i];
				}
			}
			if (cnt > 0) {
				mean /= cnt;
			}

			mu = (1 - m_c) * mu + m_c * mean;
		}
		for (size_t i = 0; i < size(); i++) {
			mv_CR[i] = GET_RND(id_rnd).normal.nextNonStd(mu, 0.01);
			if (mv_CR[i] < 0) mv_CR[i] = 0;
			else if (mv_CR[i] > 1) mv_CR[i] = 1;
		}
	}
}

