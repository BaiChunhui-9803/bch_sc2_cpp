#include "cpso_subswarm.h"

namespace ofec {

	CPSOSwarm::CPSOSwarm(size_t pop, int id_pro) :Swarm(pop, id_pro){
		m_center.reset(new templateParticle(GET_CONOP(id_pro).numObjectives(), GET_CONOP(id_pro).numConstraints(), GET_CONOP(id_pro).numVariables()));
	}

	void CPSOSwarm::initializeParameters(int id_pro, int id_alg, int id_rnd) {
		if(GET_ALG(id_alg).name() == "CPSO"){
			m_maxW = 0.6;
			m_minW = 0.3;
		}
		else if(GET_ALG(id_alg).name() == "CPSOR"){
			m_maxW = 0.6;
			m_minW = 0.6;
		}
		m_C1 = 1.7;
		m_C2 = 1.7;

		m_W = m_maxW;
		m_u = GET_DYNCONOP(id_pro).getFrequency();
		m_converge_flag = false;
		setNeighborhood(id_rnd);
	}

	void CPSOSwarm::initialize(int id_pro, int id_rnd) {
		m_W = 0.6;
		m_C1 = 1.7;
		m_C2 = 1.7;

		m_maxW = 0.6;
		m_minW = 0.3;
		m_u = GET_DYNCONOP(id_pro).getFrequency();
		setNeighborhood(id_rnd);
		m_converge_flag = false;
		for (size_t i(0); i < this->size(); i++) (*this)[i].initialize(i, id_pro, id_rnd);
		updateCurRadius(id_pro);
		initializeRadius();
	}

	void CPSOSwarm::initializeRadius() {
		m_initradius = m_radius;
	}

	int CPSOSwarm::localSearch(int id_pro, int id_alg, int id_rnd, int pop_size) {
		int rf = kNormalEval;
		//generate a permutation of templateParticle index
		std::vector<int> rindex(this->m_inds.size());
		std::iota(rindex.begin(), rindex.end(), 0);
		GET_RND(id_rnd).uniform.shuffle(rindex.begin(), rindex.end());
		//this->setNeighborhood(id_rnd);

		bool flag = false;
		for (int i = 0; i < this->m_inds.size(); i++) {
			auto& x = neighborhoodBest(rindex[i], id_pro);

			this->m_inds[rindex[i]]->nextVelocity(&x, m_W, m_C1, m_C2, id_rnd);
			this->m_inds[rindex[i]]->move();
			this->m_inds[rindex[i]]->clampVelocity(id_pro,id_rnd);

			rf = this->m_inds[rindex[i]]->evaluate(id_pro, id_alg);
			if (rf != kNormalEval) return rf;
			if (this->m_inds[rindex[i]]->dominate(this->m_inds[rindex[i]]->pbest(), GET_PRO(id_pro).optMode())) {
				this->m_inds[rindex[i]]->pbest() = this->m_inds[rindex[i]]->solut();
				rf = learnGbest(this->m_inds[rindex[i]], this->m_inds[best_idx(id_pro)], id_pro, id_alg);
				if (rf != kNormalEval) return rf;
				if (this->updateBest(this->m_inds[rindex[i]]->solut(), id_pro))
					flag = true;
			}
		}
		if (rf == kNormalEval) {
			m_flag_best_impr = flag;
			this->m_iter++;
			Real ratio = 0.;
			m_evals = GET_ALG(id_alg).numEffectiveEvals() % m_u;
			ratio = (m_iter) / ((m_u - m_evals) / static_cast<Real> (pop_size));
			if (ratio > 1.0) ratio = 1.0;
			m_W = m_maxW - (m_maxW - m_minW) * ratio;
		}
		updateCurRadius(id_pro);
		return rf;
	}

	void CPSOSwarm::setNeighborhood(int id_rnd) {
		m_link.assign(this->m_inds.size(), std::vector<bool>(this->m_inds.size(), true));
	}

	int CPSOSwarm::learnGbest(const std::unique_ptr<templateParticle>& pi, std::unique_ptr<templateParticle>& gbest,
		int id_pro, int id_alg) const{
		int rf = kNormalEval;

		size_t num_obj = GET_CONOP(id_pro).numObjectives();
		size_t num_con = GET_CONOP(id_pro).numConstraints();
		size_t num_var = GET_CONOP(id_pro).numVariables();
		for (size_t i(0); i < num_var; i++) {
			std::unique_ptr<templateParticle> temp_templateParticle = std::make_unique<templateParticle>(num_obj, num_con, num_var);
			*temp_templateParticle = *gbest;

			temp_templateParticle->pbest().variable()[i] = pi->variable()[i];
			rf = temp_templateParticle->pbest().evaluate(id_pro, id_alg, true);
			if (temp_templateParticle->pbest().dominate(gbest->pbest(), id_pro)) {
				gbest->pbest() = temp_templateParticle->pbest();
//				gbest->solut() = gbest->pbest();
			}
			if (rf != kNormalEval) return rf;
		}
		return rf;
	}

	int CPSOSwarm::best_idx(int id_pro) {
		int l = 0;
		for (int i = 0; i < this->m_inds.size(); ++i) {
			if (this->m_inds[i]->pbest().dominate(this->m_inds[l]->pbest(), id_pro)) {
				l = i;
			}
		}
		return l;
	}

	void CPSOSwarm::computeCenter(int id_pro) {
		if (this->size() < 1) return;
		if (GET_PRO(id_pro).hasTag(ProTag::kConOP)) {
			for (size_t i = 0; i < GET_CONOP(id_pro).numVariables(); i++) {
				double x = 0.;
				for (int j = 0; j < this->size(); j++) {
					auto chr = this->m_inds[j]->pbest().variable();
					x = chr[i] + x;
				}
				m_center->variable()[i] = x / this->size();
			}
			//m_center.evaluate(true, caller::Algorithm);
		}
	}

	void CPSOSwarm::updateCurRadius(int id_pro) {
		m_radius = 0;
		computeCenter(id_pro);
		if (this->size() < 2) return;
		for (int j = 0; j < this->size(); j++) m_radius += this->m_inds[j]->pbest().variableDistance(*m_center, id_pro);
		m_radius = m_radius / this->size();
	}

	void CPSOSwarm::checkOverCrowd(size_t max_sub_size, int id_pro, int id_rnd){
		if (m_converge_flag) return;
		if (size() <= max_sub_size) return;
		this->sortPbest(id_pro);
		for(size_t i(max_sub_size); i < size();i++){
			m_inds.erase(m_inds.begin() + i);
			i--;
		}
		setNeighborhood(id_rnd);
	}

	void CPSOSwarm::sortPbest(int id_pro)
	{
		templateParticle temp(GET_CONOP(id_pro).numObjectives(), GET_CONOP(id_pro).numConstraints(), GET_CONOP(id_pro).numVariables());
		for (int i = 0; i < this->size() - 1; i++) {
			for (int j = this->size() - 1; j > 0; j--) {
				if (m_inds[j]->pbest().dominate(m_inds[j - 1]->pbest(), id_pro)) {
					temp = (*m_inds[j]);
					(*m_inds[j]) = (*m_inds[j - 1]);
					(*m_inds[j - 1]) = temp;
				}
			}
		}
	}

	void CPSOSwarm::merge(CPSOSwarm& pop, int id_pro,int id_rnd)
	{
		const size_t pre_size = this->size();
		this->m_inds.resize(pre_size + pop.size());
		setNeighborhood(id_rnd);
		for (size_t i = 0; i < pop.size(); i++) {
			std::swap(this->m_inds[pre_size + i], pop.m_inds[i]);
		}
		m_initradius = (m_initradius + pop.m_initradius) / 2;
		updateCurRadius(id_pro);
	}
}
