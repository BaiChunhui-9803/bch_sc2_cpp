#include "spae.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif // !OFEC_DEMO

namespace ofec {
	void SPAE::initialize_() {
		Algorithm::initialize_();
		const auto &v = GET_PARAM(m_id_param);
		m_num_ssps = std::get<int>(v.at("initial number of subspaces"));
		m_subpop_size = std::get<int>(v.at("subpopulation size"));
		m_hlc.reset();
		m_subpops.clear();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}
	
	void SPAE::run_() {
		/* initialize attractor estimation */
		m_hlc.reset(new HLC(GET_CONOP(m_id_pro).numVariables(), m_subpop_size));
		m_hlc->initialize(GET_CONOP(m_id_pro).boundary(), GET_CONOP(m_id_pro).optMode(0), m_num_ssps);
		Solution<> tmp_sol(1, 0, GET_CONOP(m_id_pro).numVariables());
		for (size_t i = 0; i < m_num_ssps; ++i) {
			for (size_t k = 0; k < GET_CONOP(m_id_pro).numVariables(); ++k) {
				m_hlc->randomSmplInSSP(i, m_id_rnd, tmp_sol.variable().vect());
				tmp_sol.evaluate(m_id_pro, m_id_alg);
				m_hlc->inputSample(tmp_sol);
			}
		}
#ifdef OFEC_DEMO
		updateBuffer();
#endif // !OFEC_DEMO
		size_t iteration = 1;
		/* iteration begins */
		while (!terminating()) {
			/* attractor estimation */
			m_hlc->clustering();
			std::cout << "m_hlc->numBsnsAtrct(): " << m_hlc->numBsnsAtrct() << std::endl;
			updateAffiliatedSubPop();
			m_hlc->updateCurDiversity(m_subpops, m_aff_pop, m_id_pro, GET_CONOP(m_id_pro).getOptima().objective(0)[0]);
			m_hlc->updatePotential();
#ifdef OFEC_DEMO
			updateBuffer();
#endif // !OFEC_DEMO

			/* evolution */
 			for (size_t id_bsn = 0; id_bsn < m_hlc->numBsnsAtrct(); ++id_bsn) {
				if (GET_RND(m_id_rnd).uniform.next() < m_hlc->infoBsnAtrct(id_bsn).potential)
					m_subpops[m_aff_pop[id_bsn]].evolveInBsn(*m_hlc, id_bsn, m_id_pro, m_id_alg, m_id_rnd);
			}
			for (auto it = m_subpops.begin(); it != m_subpops.end();) {
				if (isSubpopConverged(**it)) {
					updateAttrctrsFnd(**it);
					it = m_subpops.remove(it);
				}
				else
					it++;
			}

#ifdef OFEC_DEMO
			updateBuffer();
#endif // !OFEC_DEMO
			iteration++;
		}
	}

	bool SPAE::isSubpopConverged(const PopType &subpop) {
		Real min_obj, max_obj;
		min_obj = max_obj = subpop[0].phenotype().objective(0);
		for (size_t i = 1; i < subpop.size(); i++) {
			if (subpop[i].phenotype().objective(0) < min_obj)
				min_obj = subpop[i].phenotype().objective(0);
			if (subpop[i].phenotype().objective(0) > max_obj)
				max_obj = subpop[i].phenotype().objective(0);
		}
		if ((max_obj - min_obj) < 0.1 * GET_PRO(m_id_pro).objectiveAccuracy()) {
			return true;
		}
		else
			return false;
	}

	void SPAE::updateAttrctrsFnd(const PopType &subpop) {
		size_t best = 0;
		for (size_t i = 1; i < subpop.size(); ++i) {
			if (subpop[i].dominate(subpop[best], m_id_pro))
				best = i;
		}
		m_hlc->updateAttractorsFnd(subpop[best], m_id_pro);
	}

	void SPAE::updateAffiliatedSubPop() {
		std::vector<size_t> aff_bsn(m_subpops.size());
		for (size_t k = 0; k < m_subpops.size(); ++k) {
			std::vector<size_t> counts(m_hlc->numBsnsAtrct(), 0);
			for (size_t i = 0; i < m_subpops[k].size(); ++i) {
				counts[m_hlc->getIdxBsn(m_subpops[k][i])]++;
			}
			aff_bsn[k] = std::max_element(counts.begin(), counts.end()) - counts.begin();
		}
		std::vector<decltype(m_subpops.begin())> aff_pop(m_hlc->numBsnsAtrct());
		size_t k = 0, id_pop=0;
		m_aff_pop.assign(m_hlc->numBsnsAtrct(), -1);
		m_aff_bsn.clear();
		for (auto it = m_subpops.begin(); it != m_subpops.end(); k++) {
			if (m_aff_pop[aff_bsn[k]] == -1) {
				aff_pop[aff_bsn[k]] = it;
				m_aff_pop[aff_bsn[k]] = id_pop;
				it++;
				id_pop++;
				m_aff_bsn.push_back(aff_bsn[k]);
			}
			else {
				it = m_subpops.merge(aff_pop[aff_bsn[k]], it);
			}
		}
		for (size_t k = 0; k < m_subpops.size(); k++) {
			bool bnd_vio = true;
			while (m_subpops[k].size() > m_subpop_size) {
				if (bnd_vio) {
					for (auto it = m_subpops[k].begin(); it != m_subpops[k].end();) {
						if (m_hlc->getIdxBsn(**it) != m_aff_bsn[k])
							it = m_subpops[k].remove(it);
						else
							it++;
					}
					if (m_subpops[k].size() == 0)
						return;
					bnd_vio = false;
					continue;
				}
				auto it_worst = m_subpops[k].begin();
				for (auto it = m_subpops[k].begin(); it != m_subpops[k].end();++it)
					if ((*it_worst)->dominate(**it, m_id_pro))
						it_worst = it;
				m_subpops[k].remove(it_worst);
			}
		}
		for (size_t id_bsn = 0; id_bsn < m_hlc->numBsnsAtrct(); id_bsn++)
			if (m_aff_pop[id_bsn] == -1)
				initSubpopInBsn(id_bsn);
	}

	void SPAE::initSubpopInBsn(size_t id_bsn) {
		auto new_pop = std::make_unique<PopType>(m_subpop_size, m_id_pro);
		for (size_t i = 0; i < new_pop->size(); ++i) {
			m_hlc->randomSmplByFreq(id_bsn, m_id_rnd, (*new_pop)[i].variable().vect());
			(*new_pop)[i].evaluate(m_id_pro, m_id_alg);
			m_hlc->inputSample((*new_pop)[i]);
		}
		m_aff_pop[id_bsn] = m_subpops.size();
		m_subpops.append(new_pop);
	}

	bool SPAE::isOverVisited(const Solution<> &sol, size_t id_bsn) {
		bool flag = false;
		for (auto &atrctr : m_hlc->attractorsFnd()) {
			if (m_hlc->getIdxBsn(atrctr) != id_bsn)
				continue;

		}
		return flag;
	}

#ifdef OFEC_DEMO
	void SPAE::updateBuffer() {
		m_solution.clear();
		m_solution.resize(m_subpops.size());
		for (size_t k = 0; k < m_subpops.size(); ++k) {
			for (size_t i = 0; i < m_subpops[k].size(); ++i) {
				m_solution[k].push_back(&m_subpops[k][i]);
			}
		}
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif // !OFEC_DEMO

	int SPAE::PopExploit::evolveInBsn(HLC &hlc, int cur_bsn, int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		std::vector<size_t> ridx;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			do {
				select(i, 3, ridx, id_rnd);
				m_inds[i]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), id_pro);
				recombine(i, id_rnd, id_pro);
			} while (GET_RND(id_rnd).uniform.next() > 1.0 / (m_iter + 1)
				&& hlc.getIdxBsn(m_inds[i]->trial()) != cur_bsn);
			tag = m_inds[i]->select(id_pro, id_alg);
			hlc.inputSample(m_inds[i]->trial());
			hlc.inputSurvivor(m_inds[i]->solut());
			if (tag == EvalTag::kTerminate) break;
		}
		if (tag == kNormalEval)
			m_iter++;
		return tag;
	}
}