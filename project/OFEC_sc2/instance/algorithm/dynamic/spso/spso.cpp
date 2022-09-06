#include "spso.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void SPSO::initialize_() {
		MetricsDynamicConOEA::initialize_();
		auto& v = GET_PARAM(m_id_param);
		int numvar = GET_DYNCONOP(m_id_pro).numVariables();
		int numpeak = GET_DYNCONOP(m_id_pro).getNumPeak();
		Real rs = 0.;
		for (int i(0);i< numvar;i++){
			auto u = GET_DYNCONOP(m_id_pro).domain().range(i).limit.first;
			auto l = GET_DYNCONOP(m_id_pro).domain().range(i).limit.second;
			rs += (l - u) * (l - u);
		}
		rs = sqrt(rs) / (2 * pow(numpeak, 1. / numvar));
		m_all_indisize = v.count("population size") > 0 ? std::get<int>(v.at("population size")) : 100;
		m_seedList.resize(v.count("population size") > 0 ? std::get<int>(v.at("population size")) : 100);
		m_rs = v.count("Separate Radius") > 0 ? std::get<Real>(v.at("Separate Radius")) : rs;
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void SPSO::initializeOriginalPop(int num){
		int rf = kNormalEval;
		int origin_size = m_all_indis.size();
		//m_all_indis.resize(num + origin_size);
		for (int i(origin_size); i < origin_size + num; ++i) {
			SPSOParticle new_particle(GET_CONOP(m_id_pro).numObjectives(), GET_CONOP(m_id_pro).numConstraints(), GET_CONOP(m_id_pro).numVariables());
			new_particle.initialize(i, m_id_pro, m_id_rnd);
			new_particle.initVelocity(m_id_pro, m_id_rnd);
			rf = new_particle.evaluate(m_id_pro, m_id_alg);
			new_particle.pbest() = new_particle.solut();
			m_all_indis.push_back(new_particle);
			if (rf != kNormalEval) 
				measureAllParticles(true);
		}
	}

	void SPSO::sortParticles(){
		SPSOParticle temp(GET_CONOP(m_id_pro).numObjectives(), GET_CONOP(m_id_pro).numConstraints(), GET_CONOP(m_id_pro).numVariables());
		for (int i = 0; i < m_all_indis.size() - 1; i++) {
			for (int j = m_all_indis.size() - 1; j > 0; j--) {
				if (m_all_indis[j].pbest().dominate(m_all_indis[j-1].pbest(),m_id_pro)) {
					temp = m_all_indis[j];
					m_all_indis[j] = m_all_indis[j-1];
					m_all_indis[j -1] = temp;
				}
			}
		}
		//for (int i(0); i < m_all_indis.size(); i++) {
		//	std::cout << m_all_indis[i].pbest().objective()[0] << std::endl;
		//}
		//std::cout << std::endl;
	}

	void SPSO::informChange(int rf){
		if (rf & kChangeCurEval) {
			measureMultiPop(true);
		}
	}

	void SPSO::measureMultiPop(bool flag){
		for (int i(0); i < m_sub_pop.size(); ++i) {
			for (int j(0); j < m_sub_pop[i].size(); j++) {
				m_sub_pop[i][j].pbest().evaluate(m_id_pro, m_id_alg, flag);
			}
		}
		for (size_t i(0); i < m_remain_indis.size(); i++) {
			m_remain_indis[i].pbest().evaluate(m_id_pro, m_id_alg, flag);
		}
	}

	void SPSO::measureAllParticles(bool flag) {
		for (int i(0); i < m_all_indis.size(); ++i) {
			m_all_indis[i].pbest().evaluate(m_id_pro, m_id_alg, flag);
		}
		for (size_t i(0); i < m_remain_indis.size(); i++) {
			m_remain_indis[i].pbest().evaluate(m_id_pro, m_id_alg, flag);
		}
	}

	void SPSO::getAllParticles(){
		if (first_ajustment) {
			first_ajustment = false;
			return;
		}
		m_all_indis.clear();

		//std::cout << "remain individual: " << m_remain_indis.size() << std::endl;
		//std::cout << "replace individual: " << m_replace_indis.size() << std::endl;

		//! remain individuals
		for (size_t i(0); i < m_remain_indis.size(); i++) {
			m_all_indis.push_back(m_remain_indis[i]); 
			m_all_indis.back().setSeed(false);
			m_all_indis.back().setSpecies(false);
		}
		m_remain_indis.clear();
		//! subpop individuals
		int count = 0;
		for (size_t i(0); i < m_sub_pop.size(); i++) {
			count += m_sub_pop[i].size();
			for (size_t j(0); j < m_sub_pop[i].size(); j++) {
				m_all_indis.push_back(m_sub_pop[i][j]);
				m_all_indis.back().setSeed(false);
				m_all_indis.back().setSpecies(false);
			}
			m_sub_pop.remove(m_sub_pop.begin() + i);
			i--;
		}
		//std::cout << "subpop individual: " << count << std::endl;
		//std::cout << std::endl;
		//! replace individuals
		for (size_t i(0); i < m_replace_indis.size(); i++) {
			m_all_indis.push_back(m_replace_indis[i]);
			m_all_indis.back().setSeed(false);
			m_all_indis.back().setSpecies(false);
		}
		m_replace_indis.clear();
	}

	void SPSO::createSpecies() {
		getAllParticles();
		sortParticles();
		m_all_indis[0].setSeed(true);
		m_all_indis[0].setSpecies(true);
		m_seed.clear();
		m_species.clear();
		m_seed.push_back(m_all_indis[0]);				//best particle becomes a seed
		m_species.push_back({ m_all_indis[0] });

		for (size_t i(0); i < m_all_indis.size(); i++) {
			for (size_t j(0); j < m_seed.size(); j++) {
				if (m_all_indis[i].isSeed()) break;
				if ((!m_all_indis[i].isSeed() && m_all_indis[i].pbest().variableDistance(m_seed[j].pbest(), m_id_pro) <= m_rs)) {
					m_all_indis[i].setSeed(false);
					m_all_indis[i].setSpecies(true);
					m_species[j].push_back(m_all_indis[i]);
					break;
				}
				else if (!m_all_indis[i].isSeed() && m_all_indis[i].pbest().variableDistance(m_seed[j].pbest(), m_id_pro) > m_rs) {
					if (j == m_seed.size() - 1) {
						m_all_indis[i].setSeed(true);
						m_all_indis[i].setSpecies(true);
						m_seed.push_back(m_all_indis[i]);
						m_species.push_back({ m_all_indis[i] });
						m_num_seeds++;
					}
				}
			}
		}

		for (size_t i(0); i < m_seed.size(); i++) {
			if (m_species[i].size() < 2) {
				m_remain_indis.push_back(m_species[i][0]);
				continue;
			}
			auto newPop = std::make_unique<SPSOSwarm>(m_species[i].size(), m_id_pro);
			newPop->W() = 0.729844;
			newPop->C1() = 1.49618;
			newPop->C2() = 1.49618;
			newPop->setNeighborhood(m_id_rnd);
			for (size_t j(0); j < m_species[i].size(); j++) {
				(*newPop)[j] = m_species[i][j];
			}
			m_sub_pop.append(newPop);
		}
	}

	void SPSO::updateReplaceParticles(){
		if (m_replace_indis.empty()) return;
		int rf = kNormalEval;
		for (int i(0); i < m_replace_indis.size(); i++) {
			m_replace_indis[i].setSeed(false);
			m_replace_indis[i].setSpecies(false);
			m_replace_indis[i].initialize(i, m_id_pro, m_id_rnd);
			//m_replace_indis[i].initVelocity(m_id_pro, m_id_rnd);
			rf = m_replace_indis[i].evaluate(m_id_pro, m_id_alg, true);
			m_replace_indis[i].pbest() = m_replace_indis[i].solut();
			if (rf != kNormalEval) {
				measureMultiPop(true);
				for(int j(0);j<=i;j++){
					m_replace_indis[j].initialize(j, m_id_pro, m_id_rnd);
					//m_replace_indis[i].initVelocity(m_id_pro, m_id_rnd);
					m_replace_indis[j].evaluate(m_id_pro, m_id_alg, false);
				}
			}
		}
	}

	bool SPSO::removeConvergence(){
		if (!m_sub_pop.size()) return false;
		for (size_t i(0); i < m_sub_pop.size(); i++) {
			for (size_t j(0); j < m_sub_pop[i].size(); j++) {
				const int best_id = m_sub_pop[i].best_idx(m_id_pro);
				if (static_cast<size_t>(best_id) == j) continue;
				if (m_sub_pop[i][j].pbest().objectiveDistance(m_sub_pop[i][best_id].pbest()) < 10e-3) {
				// if (m_sub_pop[i][j].pbest().objective()[0] == m_sub_pop[i][best_id].pbest().objective()[0]) {
					m_replace_indis.push_back(m_sub_pop[i][j]);
					m_sub_pop[i].remove(j + m_sub_pop[i].begin());
					j--;
				}
			}
		}
		return true;
	}

	void SPSO::checkOvercrowd(){
		for (size_t i(0); i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].size() > m_pMax) {
				for (size_t j(m_pMax); j < m_sub_pop[i].size(); j++) {
					m_replace_indis.push_back(m_sub_pop[i][j]);
					m_sub_pop[i].remove(m_sub_pop[i].begin() + j);
					j--;
				}
			}
		}
	}

	void SPSO::run_() {
		initializeOriginalPop(m_all_indisize);
		while (!terminating()) {
#ifdef OFEC_DEMO
			m_gbestslocation.clear();
#endif
			int rf = kNormalEval;
			createSpecies();
			checkOvercrowd();
			//std::cout << "Start : " << m_effective_eval << std::endl;
			for (size_t i(0); i < m_sub_pop.size(); i++) {
				rf = m_sub_pop[i].evolve(m_id_pro, m_id_alg, m_id_rnd);
				informChange(rf);
			}
			removeConvergence();
			updateReplaceParticles();
			//std::cout << "- : " << m_remain_indis.size() << std::endl;
			//std::cout << "End : " << m_effective_eval << std::endl;
			//std::cout << std::endl;
#ifdef OFEC_DEMO
			for (size_t i(0); i < m_sub_pop.size(); i++) m_gbestslocation.push_back(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest());
			updateBuffer();
#endif
		}
	}

	void SPSO::record(){
		std::vector<Real> entry;
		entry.push_back(m_offline_error);
		dynamic_cast<RecordVecRealDynamic&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void SPSO::updateBuffer(){
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

	std::vector<bool> SPSO::getPopHiberState() const {
		std::vector<bool> hiber_state;
		return hiber_state;
	}
#endif
}
