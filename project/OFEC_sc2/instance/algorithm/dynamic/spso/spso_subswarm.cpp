#include "spso_subswarm.h"

namespace ofec {

	SPSOSwarm::SPSOSwarm(size_t pop, int id_pro) :Swarm(pop, id_pro){
		
	}

	void SPSOSwarm::initialize(int id_pro, int id_rnd){
		m_W = 0.729844;
		m_C1 = 1.49618;
		m_C2 = 1.49618;
		setNeighborhood(id_rnd);
		Swarm<SPSOParticle>::initialize(id_pro,id_rnd);
		//this->evaluate(id_pro, id_alg);
		//this->initPbest();
	}

	void SPSOSwarm::findpopSeed(){}

	int SPSOSwarm::evolve(int id_pro, int id_alg, int id_rnd){
		int rf = kNormalEval;
		//generate a permutation of particle index
		std::vector<int> rindex(this->m_inds.size());
		std::iota(rindex.begin(), rindex.end(), 0);
		GET_RND(id_rnd).uniform.shuffle(rindex.begin(), rindex.end());
		bool flag = false;
		for (int i = 0; i < this->m_inds.size(); i++) {
			auto& x = neighborhoodBest(rindex[i], id_pro);
			this->m_inds[rindex[i]]->nextVelocity(&x, m_W, m_C1, m_C2, id_rnd);
			this->m_inds[rindex[i]]->move();
			this->m_inds[rindex[i]]->clampVelocity(id_pro,id_rnd);
			rf = this->m_inds[rindex[i]]->evaluate(id_pro, id_alg);
			if (this->m_inds[rindex[i]]->dominate(this->m_inds[rindex[i]]->pbest(), GET_PRO(id_pro).optMode())) {
				this->m_inds[rindex[i]]->pbest() = this->m_inds[rindex[i]]->solut();
				if (this->updateBest(this->m_inds[rindex[i]]->solut(), id_pro))
					flag = true;
			}
			if (rf != kNormalEval) break;
		}
		m_flag_best_impr = flag;
		this->m_iter++;
		return rf;
	}

	int SPSOSwarm::best_idx(int id_pro) {
		int l = 0;
		for (int i = 0; i < this->m_inds.size(); ++i) {
			if (this->m_inds[i]->pbest().dominate(this->m_inds[l]->pbest(), id_pro)) {
				l = i;
			}
		}
		return l;
	}

	void SPSOSwarm::setNeighborhood(int id_rnd){
		m_link.assign(this->m_inds.size(), std::vector<bool>(this->m_inds.size(), true));
	}
}