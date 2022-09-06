#include "cpso.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void CPSO::initialize_() {
		MetricsDynamicConOEA::initialize_();
		auto& v = GET_PARAM(m_id_param);
		//int numvar = GET_DYNCONOP(m_id_pro).numVariables();
		//int numpeak = GET_DYNCONOP(m_id_pro).getNumPeak();
		m_init_popsize = v.count("population size") > 0 ? std::get<int>(v.at("population size")) : 70;
		m_overlapDegree = v.count("overlap degree") > 0 ? std::get<Real>(v.at("overlap degree")) : 0.7;
		m_rconv = v.count("converge radius") > 0 ? std::get<Real>(v.at("converge radius")) : 10e-3;
		m_max_subpopSize = v.count("max subpop size") > 0 ? std::get<int>(v.at("max subpop size")) : 3;

		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void CPSO::measureMultiPop(bool flag){
		for (int i(0); i < m_sub_pop.size(); ++i) {
			for (int j(0); j < m_sub_pop[i].size(); j++) {
				m_sub_pop[i][j].pbest().evaluate(m_id_pro, m_id_alg, flag);
			}
		}
		for (int i(0); i < m_all_indis.size(); ++i) {
			m_all_indis[i]->pbest().evaluate(m_id_pro, m_id_alg, flag);
		}
	}

	int CPSO::initializeOriginalPop(int num) {
		int rf = kNormalEval;
		const size_t num_obj = GET_CONOP(m_id_pro).numObjectives();
		const size_t num_con = GET_CONOP(m_id_pro).numConstraints();
		const size_t num_var = GET_CONOP(m_id_pro).numVariables();
		int origin_size = m_all_indis.size();
		m_all_indis.resize(num + origin_size);

		for (int i(origin_size); i < m_all_indis.size(); ++i) {
			m_all_indis[i].reset(new templateParticle(num_obj, num_con, num_var));
			m_all_indis[i]->initialize(i, m_id_pro, m_id_alg);
			m_all_indis[i]->initializeVmax(m_id_pro,m_id_rnd);
			m_all_indis[i]->initVelocity(m_id_pro, m_id_rnd);
			rf = m_all_indis[i]->evaluate(m_id_pro, m_id_alg, true);
			if (rf & kChangeCurEval) return rf;
			m_all_indis[i]->pbest() = m_all_indis[i]->solut();
		}

		return rf;
	}

	int CPSO::createSubswarms(int num) {
		int rf = kNormalEval;
		rf = initializeOriginalPop(num);
		if (rf & kChangeCurEval) return rf;
		HSLH<templateParticle> cluster(m_all_indis,m_id_pro);
		cluster.clusteringRough(m_max_subpopSize,m_min_subpopSize,m_id_pro);
		for(int i(0);i<cluster.size();i++){
			std::unique_ptr<CPSOSwarm> new_pop = std::make_unique<CPSOSwarm>(cluster[i].size(), m_id_pro);
			auto iter = cluster[i].begin();
			for (int j(0); j < cluster[i].size(); ++j, ++iter) {
				(*new_pop)[j] = *(iter->second);
			}
			new_pop->initializeParameters(m_id_pro, m_id_alg, m_id_rnd);
			new_pop->updateCurRadius(m_id_pro);
			new_pop->initializeRadius();
			m_sub_pop.append(new_pop);
		}
		m_all_indis.clear();

#ifdef OFEC_DEMO
				for (size_t i(0); i < m_sub_pop.size(); i++) m_gbestslocation.push_back(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest());
				updateBuffer();
#endif
		return rf;
	}

	int CPSO::checkOverlapping() {
		for (size_t i = 0; i < m_sub_pop.size(); i++) {
			if (this->m_sub_pop[i].getConverge()) continue;
			if (this->m_sub_pop[i].size() == 0) continue;
			for (size_t j = i + 1; j < m_sub_pop.size(); j++) {
				if (this->m_sub_pop[j].getConverge()) continue;
				if (this->m_sub_pop[j].size() == 0) continue;
				const int best_i_inx = m_sub_pop[i].best_idx(m_id_pro);
				const int best_j_inx = m_sub_pop[j].best_idx(m_id_pro);
				Real dist = GET_PRO(m_id_pro).variableDistance(m_sub_pop[i][best_i_inx], m_sub_pop[j][best_j_inx]);
				if (dist < m_sub_pop[i].getInitCurRadius() && dist < m_sub_pop[j].getInitCurRadius()) {
					int c1(0);
					int c2(0);
					for (size_t k = 0; k < m_sub_pop[j].size(); k++) {
						dist = GET_PRO(m_id_pro).variableDistance(*m_sub_pop[i].center(), m_sub_pop[j][k]);
						if (dist < m_sub_pop[i].getInitCurRadius()) c1++;
					}

					for (size_t k = 0; k < m_sub_pop[i].size(); k++) {
						dist = GET_PRO(m_id_pro).variableDistance(*m_sub_pop[j].center(), m_sub_pop[i][k]);
						if (dist < m_sub_pop[j].getInitCurRadius()) c2++;	//i->j Fixed in 24/10/2021
					}
					if(c1 > m_overlapDegree * m_sub_pop[j].size() && c2 > m_overlapDegree * m_sub_pop[i].size()){
						//i->j j->i Fixed in 24/10/2021
						int idx = -1;
						if (m_sub_pop[i][best_i_inx].pbest().dominate(m_sub_pop[j][best_j_inx].pbest(), m_id_pro)) {
							m_sub_pop[i].merge(m_sub_pop[j],m_id_pro,m_id_rnd);
							m_sub_pop.remove(m_sub_pop.begin() + j);
							idx = j;
						}
						else{
							m_sub_pop[j].merge(m_sub_pop[i], m_id_pro,m_id_rnd);
							m_sub_pop.remove(m_sub_pop.begin() + i);
							idx = i;
						}
						return idx;
					}
				}
			}
		}
		return -1;
	}

	int CPSO::checkConverging() {
		int rf = kNormalEval;
		for (size_t i(0); i < m_sub_pop.size(); i++){
			if (m_sub_pop[i].getConverge()) continue;
			if(m_sub_pop[i].getCurRadius() <m_rconv){
				m_sub_pop[i].setConverge(true);
			}
		}
		if(m_all_indis.empty()){
			for(size_t i(0); i < m_sub_pop.size();i++){
				if (!m_sub_pop[i].getConverge()) return rf;
			}
			rf = initializeOriginalPop(m_max_subpopSize);
			if (rf != kNormalEval) return rf;
			std::unique_ptr<CPSOSwarm> new_pop = std::make_unique<CPSOSwarm>(m_all_indis.size(), m_id_pro);
			new_pop->initializeParameters(m_id_pro, m_id_alg, m_id_rnd);
			for (size_t i(0); i < m_all_indis.size(); i++) {
				(*new_pop)[i] = *m_all_indis[i];
			}
			new_pop->updateCurRadius(m_id_pro);
			new_pop->initializeRadius();
			m_sub_pop.append(new_pop);
		}
		return rf;
	}

	void CPSO::run_() {
		createSubswarms(m_init_popsize);
		while (!terminating()) {
#ifdef OFEC_DEMO
			m_gbestslocation.clear();
#endif
			int rf = kNormalEval;

			for (size_t i(0); i < m_sub_pop.size(); i++) {
				if(m_sub_pop[i].getConverge()) continue;
				rf = m_sub_pop[i].localSearch(m_id_pro, m_id_alg, m_id_rnd, m_init_popsize);	//localSearch
				if (rf != kNormalEval) break;
			}
			if (rf != kNormalEval) {
				informChange(rf);
				continue;
			}

			while (checkOverlapping() != -1);
			for (int i(0); i < m_sub_pop.size(); i++) m_sub_pop[i].checkOverCrowd(m_max_subpopSize, m_id_pro, m_id_rnd);

			rf = checkConverging();
			if (rf != kNormalEval) {
				informChange(rf);
				continue;
			}

			if (!m_all_indis.empty()) {
				if(m_sub_pop[m_sub_pop.size() - 1].getConverge()){
					m_all_indis.clear();
					for (size_t i(0); i < m_sub_pop.size() - 1; i++) {
						if(m_sub_pop[m_sub_pop.size() - 1][m_sub_pop[m_sub_pop.size() - 1].best_idx(m_id_pro)].pbest().variableDistance(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest(),m_id_pro) < 10e-3){
							m_sub_pop.remove(m_sub_pop.begin() + m_sub_pop.size() - 1);
							break;
						}
					}
				}
			}
#ifdef OFEC_DEMO
			for (size_t i(0); i < m_sub_pop.size(); i++) m_gbestslocation.push_back(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest());
			updateBuffer();
#endif
		}
	}

	void CPSO::informChange(int rf){
		if (rf & kChangeCurEval) {
			m_all_indis.clear();
			m_clst_indis.clear();
			measureMultiPop(true);
			for (size_t i(0); i < m_sub_pop.size(); i++) {
				m_clst_indis.push_back(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)]);
				m_sub_pop.remove(m_sub_pop.begin() + i);
				i--;
			}
			for (size_t i(0); i < m_clst_indis.size(); i++)
				m_all_indis.push_back(std::make_unique<templateParticle>(m_clst_indis[i]));
			int size = m_init_popsize - m_clst_indis.size();
			createSubswarms(size);
		}
	}

	void CPSO::record() {
		std::vector<Real> entry;
		entry.push_back(m_offline_error);
		entry.push_back(m_best_before_change_error);
		//entry.push_back(m_effective_eval);
		//entry.push_back(GET_DYNCONOP(m_id_pro).getCount());
		//entry.push_back(GET_DYNCONOP(m_id_pro).getOptima().objective(0)[0]);
		//entry.push_back(m_candidates.front()->objective(0));
		dynamic_cast<RecordVecRealDynamic&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	//void CPSO::updateBuffer(){
	//	int count = 0;
	//	for(size_t i(0);i<m_sub_pop.size();i++){
	//		if (!m_sub_pop[i].getConverge()) count++;
	//	}
	//	if (ofec_demo::g_buffer->idAlg() == m_id_alg) {
	//		m_solution.clear();
	//		m_solution.resize(/*m_sub_pop.size()*/count);
	//		int count_pop = 0;
	//		for (size_t k = 0; k < m_sub_pop.size(); ++k) {
	//			if (m_sub_pop[k].getConverge()) continue;
	//			for (size_t i = 0; i < m_sub_pop[k].size(); ++i) {
	//				m_solution[count_pop].push_back(&m_sub_pop[k][i].phenotype());
	//				//m_solution[k].push_back(&m_sub_pop[k][i].phenotype());
	//			}
	//			count_pop++;
	//		}
	//		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	//	}
	//}

	void CPSO::updateBuffer() {
		if (ofec_demo::g_buffer->idAlg() == m_id_alg) {
			m_solution.clear();
			m_solution.resize(m_sub_pop.size());
			for (size_t k = 0; k < m_sub_pop.size(); ++k) {
				for (size_t i = 0; i < m_sub_pop[k].size(); ++i) {
					m_solution[k].push_back(&m_sub_pop[k][i].phenotype());
				}
			}
			ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
		}
	}

	std::vector<bool> CPSO::getPopHiberState() const {
		std::vector<bool> hiber_state;
		for (size_t i(0); i < m_sub_pop.size(); i++) {
			hiber_state.push_back(m_sub_pop[i].getConverge());
		}
		return hiber_state;
	}
#endif
}
