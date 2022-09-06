#include "mqso.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void mQSO::initialize_()
	{
		MetricsDynamicConOEA::initialize_();
		auto& v = GET_PARAM(m_id_param);
		mc_init_indiSize = v.count("population size") > 0 ? (std::get<int>(v.at("population size")) / 10) * 10 : 100;
		double u, l;
		int d = 0;
		int peaks = 0;
		l = GET_CONOP(m_id_pro).range(0).first;
		u = GET_CONOP(m_id_pro).range(0).second;
		d = GET_CONOP(m_id_pro).numVariables();
		if (GET_CONOP(m_id_pro).hasTag(ProTag::kDOP)) peaks = GET_DYNCONOP(m_id_pro).getNumPeak();
		else peaks = 1;
		m_Rexcl = 0.5 * (u - l) / pow((double)peaks, 1. / d);		//paper function(14) r_excl= 0.5d_boa, we need to know how many peaks the problem have, it's not very well for Algorithm.
		m_Rconv = 1 * m_Rexcl;

		m_exclusion = true;
		m_M = mc_init_indiSize / 10;

		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void mQSO::initIndividual(int num) {
		int rf = kNormalEval;
		std::unique_ptr<MQSOSwarm> newPop(new MQSOSwarm(num, m_id_pro));
		newPop->initialize(m_id_alg, m_id_pro, m_id_rnd);
		rf = newPop->evaluate(m_id_pro, m_id_alg);
		if (rf & kChangeCurEval) {
			measureMultiPop(true);
			newPop->evaluate(m_id_pro, m_id_alg);
			newPop->initVelocity(m_id_pro, m_id_rnd);
			newPop->initPbest(m_id_pro);
		}
		newPop->initVelocity(m_id_pro, m_id_rnd);
		newPop->initPbest(m_id_pro);
		newPop->updateCurRadius(m_id_pro);
		newPop->getworstFlag() = false;
		m_sub_pop.append(newPop);
		//cout << "New Pop Initilized!" << endl;
	}

	bool mQSO::checkConvergenceAll()
	{
		for (int i = 0; i < m_M; i++) {
			if (m_sub_pop[i].getCurRadius() > m_Rconv) return false;
		}
		return true;
	}

	void mQSO::exclude()
	{
		if (m_exclusion) {
			for (int i = 0; i < m_M; i++) {
				for (int j = i + 1; j < m_M; j++) {
					auto best_i = m_sub_pop[i].bestIdx(m_id_pro);
					auto best_j = m_sub_pop[j].bestIdx(m_id_pro);
					if (m_sub_pop[i].getworstFlag() == false && m_sub_pop[j].getworstFlag() == false && m_sub_pop[i][best_i].variableDistance(m_sub_pop[j][best_j], m_id_pro) < m_Rexcl) {
						count_exclusion++;
						if (m_sub_pop[i][best_i].pbest().dominate(m_sub_pop[j][best_j].pbest(), m_id_pro)) {
							m_sub_pop[j].getworstFlag() = true;
						}
						else m_sub_pop[i].getworstFlag() = true;
					}
				}
			}
		}
	}


	void mQSO::evolve(){
		int rf = kNormalEval;
		for (int i(0); i < m_M; i++) {
			if (!m_sub_pop[i].getworstFlag()) {
				rf = m_sub_pop[i].evolve(m_id_pro, m_id_alg, m_id_rnd);
				m_sub_pop[i].updateCurRadius(m_id_pro);
			}
			if (rf & kChangeCurEval) {
				measureMultiPop(true); break;
			}
		}
	}

	void mQSO::measureMultiPop(bool flag)
	{
		for (int i(0); i < m_sub_pop.size(); i++) {
			m_sub_pop[i].getworstFlag() = false;
			for (int j(0); j < m_sub_pop[i].size(); j++) {
				m_sub_pop[i][j].pbest().evaluate(m_id_pro, m_id_alg, flag);
			}
		}
	}

	void mQSO::run_() {
		for (int i(0); i < m_M; i++) initIndividual(10);
		int iter = 0;
		while (!terminating()) {
			//std::cout << "iter#####" << iter++ << std::endl;
			evolve();
			updateSubPopBestIdx();
			m_bestsubPopIdx = updateBestSubPopFlag();
			exclude();
			autiConvergence();
			if (m_sub_pop[m_bestsubPopIdx].getworstFlag()) {
				std::cout << std::endl;
			}
			removeRedundancePop();
			//std::cout << std::endl;
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
		std::cout << "converge: " << count_converge << std::endl;
		std::cout << "exclude: " << count_exclusion << std::endl;
	}

	void mQSO::updateSubPopBestIdx() {
		for (int i(0); i < m_sub_pop.size(); i++) {
			m_sub_pop[i].updateBestIdx(m_sub_pop[i].bestIdx(m_id_pro));
			//std::cout << i << ": " << "best Idx: " << m_sub_pop[i].bestIdx(m_id_pro) << std::endl;
			//for (int j(0); j < m_sub_pop[i].size(); j++) {
			//	std::cout << m_sub_pop[i][j].objective()[0] << '\t' << std::endl;
			//}
			//std::cout << std::endl;
		}
	}

	int mQSO::updateBestSubPopFlag(){
		if (m_sub_pop.size() == 0) return -1;
		int idx = 0;
		for (int i(1); i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i][m_sub_pop[i].bestIdx(m_id_pro)].pbest().dominate(m_sub_pop[idx][m_sub_pop[idx].bestIdx(m_id_pro)].pbest(), m_id_pro)) {
				idx = i;
			}
		}
		for (int i(0); i < m_sub_pop.size(); i++) {
			if (i == idx) m_sub_pop[i].setBestSubPop(true);
			else m_sub_pop[i].setBestSubPop(false);
		}
		//std::cout << "best subpop: " << idx << std::endl;
		return idx;
	}

	int mQSO::findWorstPop()
	{
		if (m_sub_pop.size() == 0) return -1;
		int idx = 0;
		for (int i(1); i < m_sub_pop.size(); i++) {
			if (m_sub_pop[idx][m_sub_pop[idx].bestIdx(m_id_pro)].pbest().dominate(m_sub_pop[i][m_sub_pop[i].bestIdx(m_id_pro)].pbest(), m_id_pro)) {
				idx = i;
			}
		}
		return idx;
	}

	void mQSO::autiConvergence(){
		if (checkConvergenceAll()) {
			count_converge++;
			m_sub_pop[findWorstPop()].getworstFlag() = true;
		}
	}

	void mQSO::removeRedundancePop(){
		int rf = kNormalEval;
		for (int i(0); i < m_M; i++) {
			if (m_sub_pop[i].getworstFlag()) {
				rf = m_sub_pop[i].reInitialize(m_id_alg, m_id_pro, m_id_rnd);
				if (rf & kChangeCurEval) {
					measureMultiPop(true); break;
				}
			}
		}
	}

	void mQSO::record() {
		std::vector<Real> entry;
		entry.push_back(m_offline_error);
		//entry.push_back(m_effective_eval);
		//entry.push_back(GET_DYNCONOP(m_id_pro).getCount());
		//entry.push_back(GET_DYNCONOP(m_id_pro).getOptima().objective(0)[0]);
		//entry.push_back(m_candidates.front()->objective(0));
		dynamic_cast<RecordVecRealDynamic&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void mQSO::updateBuffer() {
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
#endif
}