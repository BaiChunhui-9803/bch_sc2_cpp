#include "hgea.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	void HGEA::initialize_() {
		Algorithm::initialize_();
		auto &param = GET_PARAM(m_id_param);
		if (param.count("population size") == 0)
			throw MyExcept("The population size of HGEA must be given.");
		else
			m_pop_size = std::get<int>(param.at("population size"));
		m_alpha = param.count("alpha") ? std::get<Real>(param.at("alpha")) : 0.8;
		if (m_alpha < 0 || m_alpha>1)
			throw MyExcept("The alpha of HGEA must be between 0 and 1.");
		m_population.resize(m_pop_size);
		m_offspring.resize(m_pop_size);
		m_ind_in_valley_or_peaks.resize(m_pop_size);
		m_off_in_valley_or_peaks.resize(m_pop_size);
	}

	void HGEA::run_() {
		initialization();
		while (!terminating()) {
			m_adaptor->updateValleyAndPeaks();
			m_adaptor->updateAffiliationHisSols();
			m_adaptor->updatePeaksPotentials(m_id_pro);
			updateAffiliations();
#ifdef OFEC_DEMO
			updateBuffer();
#endif
			parentsSelection();
			reproduction();
			evaluation();
			survivorSelection();
		}
	}

	void HGEA::initialization() {
		for (size_t i = 0; i < m_pop_size; i++) {
			initializeSolution(i);
			if (m_population[i]->evaluate(m_id_pro, m_id_alg) != kNormalEval)
				return;
			m_adaptor->archiveSolution(*m_population[i]);
		}
	}

	void HGEA::updateAffiliations() {
		m_aff_inds_valley.clear();
		m_aff_inds_each_peak.clear();
		m_aff_inds_each_peak.resize(m_adaptor->numPeaks());
		for (size_t i = 0; i < m_pop_size; ++i) {
			m_ind_in_valley_or_peaks[i] = m_adaptor->inValleyOrPeaks(*m_population[i]);
			if (m_ind_in_valley_or_peaks[i] == -1)
				m_aff_inds_valley.push_back(i);
			else
				m_aff_inds_each_peak[m_ind_in_valley_or_peaks[i]].push_back(i);
		}
	}

	void HGEA::parentsSelection() {
		for (size_t i = 0; i < m_pop_size; i++) {
			m_parents[i].clear();
			if (GET_RND(m_id_rnd).uniform.next() < m_alpha) { // exploitation 		
				auto &pp = m_adaptor->potentialPeaks();
				auto id_peak = GET_RND(m_id_rnd).uniform.spinWheel(pp.begin(), pp.end()) - pp.begin();
				m_off_in_valley_or_peaks[i] = id_peak;
				for (size_t i : m_aff_inds_each_peak[id_peak])
					m_parents[i].push_back(m_population[i].get());
				if (m_parents[i].size() < 5) {
					std::vector<const SolBase*> his_sols;
					for (auto sol : m_adaptor->hisSolsPeaks()[id_peak]) {
						his_sols.push_back(sol);
					}
					GET_RND(m_id_rnd).uniform.shuffle(his_sols.begin(), his_sols.end());
					for (auto it = his_sols.begin(); m_parents[i].size() < 5; it++)
						m_parents[i].push_back(*it);
				}
			}
			else {                                            // exploration
				m_off_in_valley_or_peaks[i] = -1;
				std::vector<const SolBase*> inds_in_valley(m_aff_inds_valley.size());
				for (size_t i = 0; i < m_aff_inds_valley.size(); i++)
					inds_in_valley[i] = m_population[m_aff_inds_valley[i]].get();
				auto sr = m_adaptor->uniquenessSols(inds_in_valley);
				auto id_ind = GET_RND(m_id_rnd).uniform.spinWheel(sr.begin(), sr.end()) - sr.begin();
				m_parents[i] = m_adaptor->adjacentHisSols(*inds_in_valley[id_ind], 5, m_id_rnd, m_id_pro);
			}
		}
	}

	void HGEA::reproduction() {
		for (size_t i = 0; i < m_pop_size; i++) {
			do {
				reproduceSolution(i);
			} while (m_adaptor->inValleyOrPeaks(*m_offspring[i]) != m_off_in_valley_or_peaks[i]);
		}
	}

	void HGEA::evaluation() {
		for (size_t i = 0; i < m_pop_size; i++) {
			if (m_offspring[i]->evaluate(m_id_pro, m_id_alg) != kNormalEval)
				return;
			m_adaptor->archiveSolution(*m_offspring[i]);
		}
	}

	void HGEA::survivorSelection() {
		for (size_t i = 0; i < m_pop_size; i++) {
			if (m_off_in_valley_or_peaks[i] > -1) {                    // exploitation 
				for (size_t ind : m_aff_inds_each_peak[m_off_in_valley_or_peaks[i]]) {
					if (m_offspring[i]->dominate(*m_population[ind], m_id_pro)) {
						m_adaptor->replaceSolution(*m_offspring[i], *m_population[ind]);
						break;
					}
				}
			}
			else {                                            // exploration
				size_t nearest = m_aff_inds_valley.front();
				Real min_dis = m_offspring[i]->variableDistance(*m_population[nearest], m_id_pro), dis;
				for (size_t j = 1; j < m_aff_inds_valley.size(); ++j) {
					dis = m_offspring[i]->variableDistance(*m_population[m_aff_inds_valley[j]], m_id_pro);
					if (dis < min_dis) {
						nearest = m_aff_inds_valley[j];
						min_dis = dis;
					}
				}
				if (m_offspring[i]->dominate(*m_population[nearest], m_id_pro))
					m_adaptor->replaceSolution(*m_offspring[i], *m_population[nearest]);
			}
		}
	}
}

