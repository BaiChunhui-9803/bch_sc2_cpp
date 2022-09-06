#include "mqso_subswarm.h"

#ifdef __GNUC__
#include <float.h>
#endif

namespace ofec {
	MQSOSwarm::MQSOSwarm(size_t pop, int id_pro) :Swarm(pop,id_pro){}

	void MQSOSwarm::initialize(int id_alg, int id_pro, int id_rnd)
	{
		m_W = 0.729844;
		m_C1 = 1.49618;
		m_C2 = 1.49618;
		m_Nplus = 5, m_Nq = 5, m_N = 5, m_Q = (pow(1. / 4.9, 1. / 0.6)), m_Rcloud = (0.5);	//parameters in swarm.
		m_center.reset(new Solution<>(GET_CONOP(id_pro).numObjectives(), GET_CONOP(id_pro).numConstraints(), GET_CONOP(id_pro).numVariables()));
		assignType(id_alg);
		m_swarmType = SwarmType::SWARM_FREE;
		for (auto& i : this->m_inds) i->initializeVmax(id_pro);
		setNeighborhood(id_rnd);
		for (size_t i(0); i < m_inds.size(); i++) 
			(*this)[i].initialize(i, id_pro,id_rnd);
	}

	int MQSOSwarm::reInitialize(int id_alg, int id_pro, int id_rnd){
		//m_swarmType = SwarmType::SWARM_FREE;
		int rf = kNormalEval;
		for (size_t i(0); i < m_inds.size(); i++)
			m_inds[i]->initialize(i, id_pro, id_rnd);
		rf = evaluate(id_pro, id_alg);
		if (rf != kNormalEval) return rf;
		initVelocity(id_pro, id_rnd);
		initPbest(id_pro);
		updateCurRadius(id_pro);
		m_worstFlag = false;
		return rf;
	}

	void MQSOSwarm::assignType(int id_alg) {
		if (GET_ALG(id_alg).name() == "mCPSO") {
			for (int i = 0; i < size(); i++) {
				if (i < m_N) m_inds[i]->getType() = MQSOParticle::ParticleType::PARTICLE_NEUTRAL;
				else  m_inds[i]->getType() = MQSOParticle::ParticleType::PARTICLE_CHARGED;
			}
		}
		else if (GET_ALG(id_alg).name() == "mQSO" || GET_ALG(id_alg).name() == "SAMO") {
			for (int i = 0; i < size(); i++) {
				if (i < m_N) m_inds[i]->getType() = MQSOParticle::ParticleType::PARTICLE_NEUTRAL;
				else  m_inds[i]->getType() = MQSOParticle::ParticleType::PARTICLE_QUANTUM;
			}
		}
	}

	void MQSOSwarm::setswarmType(SwarmType r)
	{
		m_swarmType = r;
	}

	int MQSOSwarm::bestIdx(int id_pro)
	{
		int l = 0;
		for (int i = 0; i < this->m_inds.size(); ++i) {
			if (this->m_inds[i]->pbest().dominate(this->m_inds[l]->pbest(),id_pro)) {
				l = i;
			}
		}
		return l;
	}

	void MQSOSwarm::computeRepulsion(const int idx,int id_pro) {
		int numVar = GET_CONOP(id_pro).numVariables();
		for (int d = 0; d < numVar; d++) {
			m_inds[idx]->getRepulse()[d] = 0;
		}
		for (int i = 0; i < size(); i++) {
			if (i != idx && m_inds[i]->getType() == MQSOParticle::ParticleType::PARTICLE_CHARGED) {
				double m = 0, x, y;
				for (int d = 0; d < numVar; d++) {
					x = m_inds[idx]->variable()[d];
					y = m_inds[i]->variable()[d];
					m += (x - y) * (x - y);
				}
				m = sqrt(m);
				m = pow(m, 3.);
				for (int d = 0; d < numVar; d++) {
					x = m_inds[idx]->variable()[d];
					y = m_inds[i]->variable()[d];
					m_inds[idx]->getRepulse()[d] += (x - y) * m_Q * m_Q / m;
				}
			}
		}

		// clamp to max<double>
		long double m = 0;
		for (int d = 0; d < numVar; d++) {
			m += m_inds[idx]->getRepulse()[d] * m_inds[idx]->getRepulse()[d];
		}
		m = sqrt(m);
#ifdef __GNUC__
		if (m > DBL_MAX) {
#elif defined _MSC_VER
		if (m > DBL_MAX) {
#endif
			for (int d = 0; d < numVar; d++) {
				m_inds[idx]->getRepulse()[d] = m_inds[idx]->getRepulse()[d] * DBL_MAX / m;
			}
		}
		for (int d = 0; d < numVar; d++) {
			if (std::isnan(m_inds[idx]->getRepulse()[d])) m_inds[idx]->getRepulse()[d] = 1;	//if calculate repulse too big(to NAN), let it be 1;
		}
	}

	void MQSOSwarm::computeCenter(int id_pro) {
		if (this->size() < 1) return;
		int neutral_count = 0;
		if (GET_PRO(id_pro).hasTag(ProTag::kConOP)) {
			for (size_t i = 0; i < GET_CONOP(id_pro).numVariables(); i++) {
				double x = 0.;
				for (int j = 0; j < this->size(); j++) {
					if (this->m_inds[j]->getType() == MQSOParticle::ParticleType::PARTICLE_NEUTRAL) {
						neutral_count++;
						auto chr = this->m_inds[j]->pbest().variable();
						x = chr[i] + x;
					}
				}
				m_center->variable()[i] = x / neutral_count;
				neutral_count = 0;
			}
			//m_center.evaluate(true, caller::Algorithm);
		}
	}

	void MQSOSwarm::updateCurRadius(int id_pro)
	{
		m_radius = 0;
		int neutral_count = 0;
		computeCenter(id_pro);
		if (this->size() < 2) return;
		for (int j = 0; j < this->size(); j++) {
			if (this->m_inds[j]->getType() == MQSOParticle::ParticleType::PARTICLE_NEUTRAL) {
				m_radius += this->m_inds[j]->pbest().variableDistance(*m_center, id_pro);
				neutral_count++;
			}
		}
		m_radius = m_radius / neutral_count;
		neutral_count = 0;
	}

	int MQSOSwarm::evolve(int id_pro, int id_alg, int id_rnd) {
		int rf = kNormalEval;
		if (this->size() < 1) return rf = kNormalEval;
		//generate a permutation of particle index
		std::vector<int> rindex(this->m_inds.size());
		std::iota(rindex.begin(), rindex.end(), 0);
		GET_RND(id_rnd).uniform.shuffle(rindex.begin(), rindex.end());
		//this->setNeighborhood(id_rnd);

		bool flag = false;
		for (int i = 0; i < this->m_inds.size(); i++) {
			auto& x = neighborhoodBest(rindex[i], id_pro);
			if (m_inds[rindex[i]]->getType() == MQSOParticle::ParticleType::PARTICLE_CHARGED) computeRepulsion(rindex[i], id_pro);
			if (m_inds[rindex[i]]->getType() != MQSOParticle::ParticleType::PARTICLE_QUANTUM) {
				this->m_inds[rindex[i]]->nextVelocity(&x, m_W, m_C1, m_C2, id_pro);
				this->m_inds[rindex[i]]->move();
			}
			if (m_inds[rindex[i]]->getType() == MQSOParticle::ParticleType::PARTICLE_QUANTUM) {
				m_inds[rindex[i]]->quantumMove(&x, m_Rcloud, id_pro);
			}
			this->m_inds[rindex[i]]->clampVelocity(id_pro, id_rnd);
			rf = this->m_inds[rindex[i]]->evaluate(id_pro, id_alg);

			//rf = m_inds[i]->moveto(x, m_weight, m_C1, m_C2, m_Rcloud);
			//if (rf != kNormalEval) break;
			if (this->m_inds[rindex[i]]->dominate(this->m_inds[rindex[i]]->pbest(),id_pro)) {
				this->m_inds[rindex[i]]->pbest() = this->m_inds[rindex[i]]->solut();
				if (this->updateBest(this->m_inds[rindex[i]]->solut(),id_pro))
					flag = true;
			}
			if (rf != kNormalEval) break;
			//cout << m_inds[i]->variable()[0] << " & "<< m_inds[i]->variable()[1] << endl;
		}
		m_flag_best_impr = flag;
		this->m_iter++;
		return rf;
	}

	void MQSOSwarm::setNeighborhood(int id_rnd) {
		m_link.assign(this->m_inds.size(), std::vector<bool>(this->m_inds.size(), true));
	}
}