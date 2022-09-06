#include "ftmpso_subswarm.h"

namespace ofec {

	FTMPSOSwarm::FTMPSOSwarm(size_t pop, int id_pro, SwarmType t) :Swarm(pop, id_pro), m_swarmtype(t) {
		m_center.reset(new Solution<>(GET_PRO(id_pro).numObjectives(), GET_PRO(id_pro).numConstraints(), GET_CONOP(id_pro).numVariables()));
	}

	void FTMPSOSwarm::initialize(int id_pro, int id_rnd) {
		m_W = 0.729844;
		m_C1 = 1.49618;
		m_C2 = 1.49618;
		setNeighborhood(id_rnd);
		for (size_t i(0); i < this->size(); i++) (*this)[i].initialize(i, id_pro, id_rnd);
	}

	int FTMPSOSwarm::evolve(int id_pro, int id_alg, int id_rnd) {
		int rf = kNormalEval;
		//generate a permutation of particle index
		std::vector<int> rindex(this->m_inds.size());
		std::iota(rindex.begin(), rindex.end(), 0);
		GET_RND(id_rnd).uniform.shuffle(rindex.begin(), rindex.end());
		//this->setNeighborhood(id_rnd);

		bool flag = false;
		for (int i = 0; i < this->m_inds.size(); i++) {
			auto& x = neighborhoodBest(rindex[i], id_pro);

			this->m_inds[rindex[i]]->nextVelocity(&x, m_W, m_C1, m_C2, id_rnd);
			this->m_inds[rindex[i]]->move();
			this->m_inds[rindex[i]]->clampVelocity(id_pro, id_rnd);

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

	void FTMPSOSwarm::setNeighborhood(int id_rnd) {
		m_link.assign(this->m_inds.size(), std::vector<bool>(this->m_inds.size(), true));
	}

	int FTMPSOSwarm::best_idx(int id_pro) {
		int l = 0;
		for (int i = 0; i < this->m_inds.size(); ++i) {
			if (this->m_inds[i]->pbest().dominate(this->m_inds[l]->pbest(), id_pro)) {
				l = i;
			}
		}
		return l;
	}

	void FTMPSOSwarm::computeCenter(int id_pro) {
		if (this->size() < 1) return;
		if (GET_PRO(id_pro).hasTag(ProTag::kConOP)) {
			for (size_t i = 0; i < GET_CONOP(id_pro).numVariables(); i++) {
				double x = 0.;
				for (int j = 0; j < this->size(); j++) {
					auto chr = this->m_inds[j]->variable();
					x = chr[i] + x;
				}
				m_center->variable()[i] = x / this->size();
			}
			//m_center.evaluate(true, caller::Algorithm);
		}
	}

	void FTMPSOSwarm::updateCurRadius(int id_pro) {
		m_radius = 0;
		computeCenter(id_pro);
		if (this->size() < 2) return;
		for (int j = 0; j < this->size(); j++) m_radius += this->m_inds[j]->variableDistance(*m_center, id_pro);
		m_radius = m_radius / this->size();
	}



}