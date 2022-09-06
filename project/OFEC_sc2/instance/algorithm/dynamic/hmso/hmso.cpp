#include "hmso.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void HmSO::initialize_() {
		MetricsDynamicConOEA::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_preiter.reset(new Solution<>(GET_PRO(m_id_pro).numObjectives(), GET_PRO(m_id_pro).numConstraints(), GET_CONOP(m_id_pro).numVariables()));
		m_curiter.reset(new Solution<>(GET_PRO(m_id_pro).numObjectives(), GET_PRO(m_id_pro).numConstraints(), GET_CONOP(m_id_pro).numVariables()));
		m_rpc = v.count("Radius between Parent and Child") > 0 ? std::get<Real>(v.at("Radius between Parent and Child")) : 30.;
		m_rconv = v.count("Converge Radius") > 0 ? std::get<Real>(v.at("Converge Radius")) : 1.0;
		m_rexcl = v.count("Exclusion Radius") > 0 ? std::get<Real>(v.at("Exclusion Radius")) : 30.0;
		m_rs = v.count("Search Radius") > 0 ? std::get<Real>(v.at("Search Radius")) : 0.5;
		m_margin = 5.;
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void HmSO::initializeParent(int num)
	{
		int rf = kNormalEval;
		std::unique_ptr<HmSOSwarm> newPop(new HmSOSwarm(num, m_id_pro, HmSOSwarm::SwarmType::Parent));
		for (size_t i(0); i < newPop->size(); i++) (*newPop)[i].initialize(i, m_id_pro, m_id_rnd);
		newPop->W() = 0.729844;
		newPop->C1() = 1.49618;
		newPop->C2() = 1.49618;
		newPop->setNeighborhood(m_id_rnd);
		rf = newPop->evaluate(m_id_pro, m_id_alg);
		newPop->initPbest(m_id_pro);
		informChange(rf);
		//cout << "Parent: " << newPop->neighborhoodBest(0,m_id_pro).objective()[0] << endl;
		m_sub_pop.append(newPop);
		*m_preiter = m_sub_pop[0][m_sub_pop[0].best_idx(m_id_pro)].pbest();
	}

	void HmSO::initializeChild(HmSOSwarm& parent, int num) {
		int rf;
		int count = 0;
		std::unique_ptr<HmSOSwarm> newPop(new HmSOSwarm(num, m_id_pro, HmSOSwarm::SwarmType::Child));
		newPop->W() = 0.729844;
		newPop->C1() = 1.49618;
		newPop->C2() = 1.49618;
		newPop->setNeighborhood(m_id_rnd);
		(*newPop)[0] = parent[parent.best_idx(m_id_pro)];
		count++;
		for (size_t i(0); i < parent.size(); i++) {
			if (parent[i].pbest().variableDistance(parent[parent.best_idx(m_id_pro)].pbest(), m_id_pro) < m_rpc/* && i != parent.best_idx(m_id_pro)*/) {
				if (count < num) {
					(*newPop)[count++] = parent[i];
				}
				parent[i].initialize(i, m_id_pro, m_id_rnd);
				rf = parent[i].evaluate(m_id_pro, m_id_alg);
				informChange(rf);
				if (rf != kNormalEval) break;
				parent[i].pbest() = parent[i].solut();
			}
		}
		while (count < num) {
			Particle add(GET_PRO(m_id_pro).numObjectives(), GET_PRO(m_id_pro).numConstraints(), GET_CONOP(m_id_pro).numVariables());
			//add.initialize(count, m_id_pro, m_id_rnd);
			for (size_t j = 0; j < add.variable().size(); j++) {
				Real temp = GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-1, 1);
				add.variable()[j] = newPop->neighborhoodBest(0, m_id_pro).variable()[j] + (m_rpc / 3.) * temp;
				add.velocity()[j] = GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-1,1) * (m_rpc / 3.);
			}
			rf = add.evaluate(m_id_pro, m_id_alg, true); informChange(rf);
			add.pbest() = add.solut();
			add.pbest() = add.solut();
			(*newPop)[count++] = add;
		}
		//rf = newPop->evaluate(m_id_pro, m_id_alg);
		newPop->initPbest(m_id_pro);
		m_sub_pop.append(newPop);
		//informChange(rf);
	}

	void HmSO::checkOverlapping() {
		for (size_t i = 0; i < m_sub_pop.size(); i++) {
			for (size_t j = i + 1; j < m_sub_pop.size(); j++) {
				if (m_sub_pop[i].getswarmType() == HmSOSwarm::SwarmType::Child && m_sub_pop[j].getswarmType() == HmSOSwarm::SwarmType::Child
					&& m_sub_pop[i].neighborhoodBest(0, m_id_pro).variableDistance(m_sub_pop[j].neighborhoodBest(0, m_id_pro), m_id_pro) < m_rexcl) {
					if (m_sub_pop[i].neighborhoodBest(0, m_id_pro).dominate(m_sub_pop[j].neighborhoodBest(0, m_id_pro), m_id_pro)) {
						m_sub_pop[j].setexcelFlag(true);
					}
					else m_sub_pop[i].setexcelFlag(true);
				}
			}
		}
	}

	void HmSO::removeOverlapping() {
		for (size_t i = 0; i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].getswarmType() == HmSOSwarm::SwarmType::Child && m_sub_pop[i].getexcelFlag()) {
				m_sub_pop.remove(m_sub_pop.begin() + i);
				i--;
			}
		}
	}

	void HmSO::createChildSwarm(){
		if ((*m_curiter).dominate((*m_preiter), m_id_pro)) 
			initializeChild(m_sub_pop[0], 10);
		*m_preiter = *m_curiter;

	}

	int HmSO::updateBestSubPopIdx() {
		if (m_sub_pop.size() == 0) return -1;
		int idx = 0;
		for (int i(1); i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].neighborhoodBest(0,m_id_pro).dominate(m_sub_pop[idx].neighborhoodBest(0, m_id_pro), m_id_pro)) {
				idx = i;
			}
		}
		//std::cout << "best subpop: " << idx << std::endl;
		return idx;
	}

	void HmSO::updateChildSwarm() {
		const int best_subpop_idx = updateBestSubPopIdx();
		for (auto& i : m_sub_pop)
		{
			if (i->getswarmType() == HmSOSwarm::SwarmType::Child && !i->gethiberFlag()) {
				int rf = kNormalEval;
				rf = i->evolve(m_id_pro, m_id_alg, m_id_rnd);
				informChange(rf);
				i->updateCurRadius(m_id_pro);
				//cout << m_sub_pop[i].getCurRadius() << '\t';
				if (i->getCurRadius() < m_rconv
					&& (*i)[i->best_idx(m_id_pro)].pbest().objective()[0] < (m_sub_pop[best_subpop_idx][m_sub_pop[best_subpop_idx].best_idx(m_id_pro)].pbest().objective()[0] - m_margin))
					i->sethiberFlag(true);
			}
			//cout << endl;
		}
		checkOverlapping();
		removeOverlapping();
	}

	void HmSO::updateChildAttractor(){
		int rf = kNormalEval;
		rf = m_sub_pop[0].evolve(m_id_pro, m_id_alg, m_id_rnd);
		informChange(rf);
		*m_curiter = m_sub_pop[0][m_sub_pop[0].best_idx(m_id_pro)].pbest();
		for (size_t i = 0; i < m_sub_pop[0].size(); i++) {
			for (size_t j = 1; j < m_sub_pop.size(); j++) {
				if (m_sub_pop[0][i].pbest().variableDistance(m_sub_pop[j][m_sub_pop[j].best_idx(m_id_pro)].pbest(), m_id_pro) < m_rpc) {
					if (m_sub_pop[0][i].pbest().dominate(m_sub_pop[j][m_sub_pop[j].best_idx(m_id_pro)].pbest(), m_id_pro)) {
						m_sub_pop[j][m_sub_pop[j].best_idx(m_id_pro)] = m_sub_pop[0][i];
					}
					m_sub_pop[0][i].initialize(i, m_id_pro, m_id_rnd);
					rf = m_sub_pop[0][i].evaluate(m_id_pro, m_id_alg);
					informChange(rf);
					m_sub_pop[0][i].pbest() = m_sub_pop[0][i].solut();
				}
			}
		}
		createChildSwarm();
	}

	void HmSO::wakeupHiberPop() {
		for (size_t i(0); i < m_sub_pop.size(); i++) {
			if (m_sub_pop[i].getswarmType() == HmSOSwarm::SwarmType::Child && m_sub_pop[i].gethiberFlag()) {
				m_sub_pop[i].sethiberFlag(false);
			}
			if (m_sub_pop[i].getswarmType() == HmSOSwarm::SwarmType::Child) {
				relaunchChildSwarm(m_sub_pop[i]);
			}
		}
	}

	void HmSO::relaunchChildSwarm(HmSOSwarm& child) {
		const int best_idx = child.best_idx(m_id_pro);
		for (size_t i(0); i < child.size(); i++) {
			if (i != best_idx) {
				for (size_t j(0); j < child[i].variable().size(); j++) {
					child[i].variable()[j] = child[best_idx].pbest().variable()[j] + GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-1, 1) * m_rs;
					child[i].velocity()[j] = GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-1, 1) * m_rs;

				}
				child[i].evaluate(m_id_pro, m_id_alg);
				//child[i].initVelocity(m_id_pro,m_id_rnd);
				child[i].pbest() = child[i].solut();
			}
		}
	}

	void HmSO::informChange(int rf)
	{
		if (rf & kChangeCurEval) {
			measureMultiPop(true);
			wakeupHiberPop();
		}
	}

	bool HmSO::checkParentConvergence()
	{
		bool flag = true;
		for (int j = 0; j < m_sub_pop[0].size() && flag; j++) {
			for (int k = j + 1; k < m_sub_pop[0].size() && flag; k++) {
				for (int d = 0; d < GET_CONOP(m_id_pro).numVariables(); d++) {
					if (fabs(static_cast<double>(m_sub_pop[0][j].variable()[d] - m_sub_pop[0][k].variable()[d])) > m_rconv) {
						flag = false;
						break;
					}
				}
			}
		}
		return flag;
	}

	void HmSO::preventParentConvergence(){
		int rf = kNormalEval;
		if (checkParentConvergence()) {
			for (size_t i = 0; i < m_sub_pop[0].size(); i++) m_sub_pop[0][i].initialize(i, m_id_pro, m_id_rnd);
			rf = m_sub_pop[0].evaluate(m_id_pro, m_id_alg);
			m_sub_pop[0].initPbest(m_id_pro);
			informChange(rf);
		}
	}

	void HmSO::run_() {
		initializeParent(5);
		while (!terminating()) {
#ifdef OFEC_DEMO
			m_gbestslocation.clear();
#endif
			updateChildAttractor();
			createChildSwarm();
			//preventParentConvergence();
			updateChildSwarm();
#ifdef OFEC_DEMO
			for (size_t i(0); i < m_sub_pop.size(); i++) m_gbestslocation.push_back(m_sub_pop[i][m_sub_pop[i].best_idx(m_id_pro)].pbest());
			updateBuffer();
#endif
		}
	} 

	void HmSO::measureMultiPop(bool flag)
	{
		for (int i(0); i < m_sub_pop.size(); ++i) {
			for (int j(0); j < m_sub_pop[i].size(); j++) {
				m_sub_pop[i][j].pbest().evaluate(m_id_pro, m_id_alg, flag);
			}
		}
	}

	void HmSO::record() {
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
	void HmSO::updateBuffer() {
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

	std::vector<bool> HmSO::getPopHiberState() const {
		std::vector<bool> hiber_state;
		for (size_t i(0); i < m_sub_pop.size(); i++) {
			hiber_state.push_back(m_sub_pop[i].gethiberFlag());
		}
		return hiber_state;
	}
#endif

}