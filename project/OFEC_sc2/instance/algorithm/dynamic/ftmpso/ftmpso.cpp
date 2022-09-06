#include "ftmpso.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void FTMPSO::initialize_() {
		MetricsDynamicConOEA::initialize_();
		m_xr.reset(new Particle(GET_PRO(m_id_pro).numObjectives(), GET_PRO(m_id_pro).numConstraints(), GET_CONOP(m_id_pro).numVariables()));
		auto& v = GET_PARAM(m_id_param);
		int numvar = GET_DYNCONOP(m_id_pro).numVariables();
		int numpeak = GET_DYNCONOP(m_id_pro).getNumPeak();
		m_etry = 20;
		m_rcloud = 0.2;
		m_cfmin = 0.8;
		m_rconv = v.count("Converge Radius") > 0 ? std::get<Real>(v.at("Converge Radius")) : 1.;
		m_rexcl = v.count("Exclusion Radius") > 0 ? std::get<Real>(v.at("Exclusion Radius")) : 0.5*(100./pow(numpeak, (1./numvar)));
		m_rs = v.count("Search Radius") > 0 ? std::get<Real>(v.at("Search Radius")) : 0.5;
		m_qs = v.count("Velocity Search") > 0 ? std::get<Real>(v.at("Velocity Search")) : 0.5;
		m_limit = v.count("Hibernate Limit") > 0 ? std::get<Real>(v.at("Hibernate Limit")) : 0.4;
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void FTMPSO::initializeFinder(int num){
		int rf = kNormalEval;
		auto newPop = std::make_unique<FTMPSOSwarm>(num, m_id_pro, FTMPSOSwarm::SwarmType::FINDER);
		newPop->initialize(m_id_pro,m_id_rnd);
		rf = newPop->evaluate(m_id_pro, m_id_alg);
		newPop->initVelocity(m_id_pro, m_id_rnd);
		newPop->initPbest(m_id_pro);
		informChange(rf);
		m_sub_pop.append(newPop);
	}

	int FTMPSO::initializeTracker(FTMPSOSwarm& parent, int num){
		int rf = kNormalEval;
		sortFinder(parent);
		std::unique_ptr<FTMPSOSwarm> newPop(new FTMPSOSwarm(num, m_id_pro, FTMPSOSwarm::SwarmType::TRACKER));
		newPop->W() = 0.729844;
		newPop->C1() = 1.49618;
		newPop->C2() = 1.49618;
		newPop->setNeighborhood(m_id_rnd);
		for (size_t i(0); i < newPop->size(); i++)	(*newPop)[i] = parent[parent.size() - i - 1];
		m_sub_pop.append(newPop);
		parent.initialize(m_id_pro, m_id_rnd);
		rf = parent.evaluate(m_id_pro, m_id_alg);
		parent.initPbest(m_id_pro);
		informChange(rf);
		return rf;
	}

	void FTMPSO::sortFinder(FTMPSOSwarm& parent){
		Particle temp(GET_PRO(m_id_pro).numObjectives(), GET_PRO(m_id_pro).numConstraints(), GET_CONOP(m_id_pro).numVariables());
		//std::cout << "Before sort:" << std::endl;
		//for (int i = 0; i < parent.size(); i++) {
		//	for (int j = 0; j < parent[i].pbest().variable().size(); j++) {
		//		std::cout << parent[i].pbest().variable()[j] << '\t';
		//	}
		//	std::cout << '\n' << parent[i].pbest().objective()[0] << '\n';
		//}

		for (int i = parent.size() - 1; i > 0; i--) {
			for (int j = 0; j < i; j++) {
				if (parent[j].pbest().dominate(parent[j + 1].pbest(),m_id_pro)) {
					temp = m_sub_pop[0][j + 1];
					parent[j + 1] = m_sub_pop[0][j];
					parent[j] = temp;
				}
			}
		}

		//std::cout << "After sort:" << std::endl;
		//for (int i = 0; i < parent.size(); i++) {
		//	for (int j = 0; j < parent[i].pbest().variable().size(); j++) {
		//		std::cout << parent[i].pbest().variable()[j] << '\t';
		//	}
		//	std::cout << '\n' << parent[i].pbest().objective()[0] << '\n';
		//}
	}

	int FTMPSO::getBestTracker(){
		int bestindex = -1;
		for (size_t i(1); i < m_sub_pop.size(); i++) {
			if (bestindex == -1) bestindex = i;
			if (m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest().dominate(m_sub_pop[bestindex][m_sub_pop[bestindex].best_idx(m_id_pro)].pbest(), m_id_pro)) {
				bestindex = i;
			}
		}
		return bestindex;
	}

	void FTMPSO::updateFinderGBestMemory(){
		if (m_finderGbests.size() < 3) m_finderGbests.push_back(m_sub_pop[0][m_sub_pop[0].best_idx(m_id_pro)].pbest());
		else {
			for(size_t i(0);i< m_finderGbests.size()-1;i++)
				m_finderGbests[i] = m_finderGbests[i+1];
			m_finderGbests[m_finderGbests.size() - 1] = m_sub_pop[0][m_sub_pop[0].best_idx(m_id_pro)].pbest();
		}
	}

	int FTMPSO::finderExclusion(){
		int rf = kNormalEval;
		for (size_t i = 1; i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].getexcelFlag()) continue;
			if(m_sub_pop[0][m_sub_pop[0].best_idx(m_id_pro)].pbest().variableDistance(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest(), m_id_pro) < m_rexcl) {
				m_sub_pop[0].initialize(m_id_pro, m_id_rnd);
				rf = m_sub_pop[0].evaluate(m_id_pro, m_id_alg);
				m_sub_pop[0].initPbest(m_id_pro);
				informChange(rf);									//inform change if there are evaluations.
				if (rf != kNormalEval) return rf;
			}
		}
		return rf;
	}

	void FTMPSO::trackerExclusion(){
		for (size_t i = 1; i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].getexcelFlag()) continue;
			for (size_t j = i + 1; j < m_sub_pop.size(); j++) {
				if (m_sub_pop[j].getexcelFlag()) continue;
				if (m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest().variableDistance(m_sub_pop[j][m_sub_pop[j].best_idx(m_id_pro)].pbest(), m_id_pro) < m_rexcl) {
					if (m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest().dominate(m_sub_pop[j][m_sub_pop[j].best_idx(m_id_pro)].pbest(), m_id_pro)) {
						m_sub_pop[j].setexcelFlag(true);
					}
					else m_sub_pop[i].setexcelFlag(true);
				}
			}
		}
	}

	void FTMPSO::removeExclusion() {
		if (m_sub_pop.size() < 2) return;
		for (size_t i = 1; i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].getexcelFlag()) {
				m_sub_pop.remove(m_sub_pop.begin() + i);
				i--;
			}
		}
	}

	int FTMPSO::isFinderInTracker(){
		for (decltype(m_sub_pop.size()) i = 1; i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].getexcelFlag()) continue;
			if (m_sub_pop[0][m_sub_pop[0].best_idx(m_id_pro)].pbest().variableDistance(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest(), m_id_pro) < m_rexcl) {
				return i;
			}
		}
		return -1;
	}

	int FTMPSO::activation(){
		int rf = kNormalEval;
		updateFinderGBestMemory();
		if (m_finderGbests.size() < 3) return rf;
		if (m_sub_pop[0].iteration() % 2 == 0) {
			if (m_finderGbests[2].variableDistance(m_finderGbests[0], m_id_pro) < m_rconv) {
				int idx = isFinderInTracker();
				if (idx == -1) {
					rf = initializeTracker(m_sub_pop[0], 5);
					if (rf != kNormalEval) return rf;
				}
				else if (idx != -1) {
					m_sub_pop[idx].setexcelFlag(true);
				}
			}
		}
		return rf;
	}

	int FTMPSO::trackerEvolve() {
		int rf = kNormalEval;
		if (m_sub_pop.size() > 1) {
			int bestIdx = getBestTracker();
			for (size_t i(1); i < m_sub_pop.size(); i++) {
				if (m_sub_pop[i].gethiberFlag() || m_sub_pop[i].getexcelFlag()) continue;
				rf = m_sub_pop[i].evolve(m_id_pro, m_id_alg, m_id_rnd);
				informChange(rf); if (rf != kNormalEval) return rf;
				if (i != bestIdx && judgeHiberPop(m_sub_pop[i])) m_sub_pop[i].sethiberFlag(true);
			}
		}
		return rf;
	}

	int FTMPSO::besttrackerTry(){
		int rf = kNormalEval;
		int best_popindex = getBestTracker();
		if (best_popindex != -1) {
			int effectivecount(0);
			int best_indivindex = m_sub_pop[best_popindex].best_idx(m_id_pro);
			for (size_t i(0); i < m_etry; i++) {
				for (size_t j(0); j < GET_CONOP(m_id_pro).numVariables(); j++) {
					(*m_xr).variable()[j] = m_sub_pop[best_popindex][best_indivindex].pbest().variable()[j] +
						GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-1, 1) * m_rcloud;
				}
				rf = (*m_xr).evaluate(m_id_pro,m_id_alg,true);
				if ((*m_xr).dominate(m_sub_pop[best_popindex][best_indivindex].pbest(),m_id_pro)) {
					m_sub_pop[best_popindex][best_indivindex] = (*m_xr);
					m_sub_pop[best_popindex][best_indivindex].pbest() = m_sub_pop[best_popindex][best_indivindex].solut();
					effectivecount++;
				}
				informChange(rf);
				if (rf != kNormalEval)  break;
			}
			m_cf = m_cfmin * GET_RND(m_id_rnd).uniform.next() * (1 - m_cfmin);
			m_rcloud *= m_cf;
			//cout << effectivecount << endl;
		}
		return rf;
	}

	bool FTMPSO::judgeHiberPop(FTMPSOSwarm& pop){
		bool hiberFlag = true;
		for (size_t i(0); i < pop.size(); i++) {
			for (size_t j(0); j < pop[i].velocity().size(); j++) {
				if (fabs(pop[i].velocity()[j]) > m_limit) {
					hiberFlag = false;
					break;
				}
			}
		}
		return hiberFlag;
	}

	void FTMPSO::wakeupHiberPop(){
		for (size_t i(1); i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].gethiberFlag()) {
				m_sub_pop[i].sethiberFlag(false);
			}
			relaunchTrackerSwarm(m_sub_pop[i]);
		}
	}

	void FTMPSO::relaunchTrackerSwarm(FTMPSOSwarm& tracker){
		int rf = kNormalEval;
		int best_idx = tracker.best_idx(m_id_pro);
		for (size_t i(0); i < tracker.size(); i++) {
			if (i != best_idx) {
				for (size_t j(0); j < tracker[i].variable().size(); j++) {
					tracker[i].variable()[j] = tracker[best_idx].pbest().variable()[j] + GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-1, 1) * m_rs;
					tracker[i].velocity()[j] = GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-1, 1) * m_qs;
				}
				rf = tracker[i].evaluate(m_id_pro,m_id_alg);
				tracker[i].pbest() = tracker[i].solut();
				informChange(rf);
			}
		}
	}

	void FTMPSO::informChange(int rf){
		if (rf & kChangeCurEval) {
			m_finderGbests.clear();
			measureMultiPop(true);
			wakeupHiberPop();
		}
	}

	void FTMPSO::measureMultiPop(bool flag){
		m_rcloud = 0.2;
		for (int i(0); i < m_sub_pop.size(); ++i) {
			for (int j(0); j < m_sub_pop[i].size(); j++) {
				m_sub_pop[i][j].pbest().evaluate(m_id_pro, m_id_alg, flag);
			}
		}
	}

	void FTMPSO::run_() {
		initializeFinder(10);
		while (!terminating()) {
#ifdef OFEC_DEMO
			m_gbestslocation.clear();
#endif
			int rf = kNormalEval;
			rf = m_sub_pop[0].evolve(m_id_pro, m_id_alg, m_id_rnd);	//finder evolve
			informChange(rf);		//inform environment change
			finderExclusion();		//remove exclusion finder swarm
			activation();		//activate tracker swarm
			trackerEvolve();							//tracker evolve
			besttrackerTry();
			trackerExclusion();
			removeExclusion();
#ifdef OFEC_DEMO
			for (size_t i(0); i < m_sub_pop.size(); i++) m_gbestslocation.push_back(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest());
			updateBuffer();
#endif
		}
	}

	void FTMPSO::record(){
		std::vector<Real> entry;
		entry.push_back(m_offline_error);
		//entry.push_back(m_effective_eval);
		//entry.push_back(GET_DYNCONOP(m_id_pro).getCount());
		//entry.push_back(GET_DYNCONOP(m_id_pro).getOptima().objective(0)[0]);
		//entry.push_back(m_candidates.front()->objective(0));
		dynamic_cast<RecordVecRealDynamic&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void FTMPSO::updateBuffer(){
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

	std::vector<bool> FTMPSO::getPopHiberState() const {
		std::vector<bool> hiber_state;
		for (size_t i(0); i < m_sub_pop.size(); i++) {
			hiber_state.push_back(m_sub_pop[i].gethiberFlag());
		}
		return hiber_state;
	}
#endif
}
