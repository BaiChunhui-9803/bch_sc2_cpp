#include "lips_pop.h"
#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	LI_particle::LI_particle(size_t num_obj, size_t num_con, size_t size_var) :
		Particle(num_obj, num_con, size_var),
		m_pos(size_var),
		m_rdm(size_var)
	{
		setImproved(true);
	}

	void LI_particle::nextVelocityByWeight(Real w) {
		for (size_t j = 0; j < m_var.size(); j++) {
			m_vel[j] = w * (m_vel[j] + m_rdm[j] * (m_pos[j] - m_var[j]));
		}
	}

	SwarmLIP::SwarmLIP(size_t size_pop, int id_pro, int max_evals) :
		Swarm(size_pop, id_pro),
		m_M(2),
		m_max_evals(max_evals),
		m_dis(size_pop) {}

	int SwarmLIP::evolve(int id_pro, int id_alg, int id_rnd) {
		int rf = kNormalEval;
		// Update the value of M
		if (m_iter * m_inds.size() > m_max_evals)
			m_M = 5;
		else
			m_M = 2 + 3. * static_cast<Real>(m_iter * m_inds.size()) / static_cast<Real>(m_max_evals);
		// Breeding process
		m_flag_best_impr = false;
		for (int i = 0; i < m_inds.size(); i++) {
			sortDistance(i, id_pro);
			setBestPos(i, id_pro, id_rnd);
			m_inds[i]->nextVelocityByWeight(m_W);   // lbest and c1, c2 are actually not used in LIPS
			m_inds[i]->move();
			m_inds[i]->clampVelocity(id_pro, id_rnd);
			rf = m_inds[i]->evaluate(id_pro, id_alg);
			if (m_inds[i]->dominate(m_inds[i]->pbest(), id_pro)) {
				m_inds[i]->pbest() = m_inds[i]->solut();
				if (updateBest(m_inds[i]->solut(), id_pro))
					m_flag_best_impr = true;
				m_inds[i]->setImproved(true);
			}
			else
				m_inds[i]->setImproved(false);
			handleEvalTag(rf);
			if (rf != kNormalEval) break;
		}
		this->m_iter++;
		return rf;
	}

	void SwarmLIP::setBestPos(int idx_ind, int id_pro, int id_rnd) {
		for (auto& j : m_inds[idx_ind]->m_pos) j = 0;
		for (auto& j : m_inds[idx_ind]->m_rdm) j = 0;
		for (auto& i : m_inds[idx_ind]->m_nbr) {
			for (auto j = 0; j < GET_CONOP(id_pro).numVariables(); j++) {
				double rdom = GET_RND(id_rnd).uniform.next() * (4.1 / m_M);
				m_inds[idx_ind]->m_pos[j] += m_inds[i]->pbest().variable()[j] * rdom;
				m_inds[idx_ind]->m_rdm[j] += rdom;
			}
		}
		for (auto j = 0; j < GET_CONOP(id_pro).numVariables(); ++j) {
			m_inds[idx_ind]->m_pos[j] = m_inds[idx_ind]->m_pos[j] / m_inds[idx_ind]->m_rdm[j];
		}
	}

	void SwarmLIP::sortDistance(int idx_ind, int id_pro) {
		for (size_t j = 0; j < m_inds.size(); j++) {
			if (idx_ind == j) continue;
			if (m_inds[idx_ind]->isImproved() || m_inds[j]->isImproved()) {
				std::pair<Real, size_t> dis = std::make_pair(m_inds[idx_ind]->variableDistance(*m_inds[j], id_pro), j);
				auto it = m_dis[idx_ind].begin();
				while (it != m_dis[idx_ind].end() && it->first < dis.first) {
					it++;
				}
				if (m_dis[idx_ind].size() >= m_M - 1) {
					if (it != m_dis[idx_ind].end()) {
						m_dis[idx_ind].insert(it, dis);
						m_dis[idx_ind].pop_back();
					}
				}
				else {
					m_dis[idx_ind].insert(it, dis);
				}
			}
		}
		m_inds[idx_ind]->m_nbr.resize(0);
		for (auto& i : m_dis[idx_ind])
			m_inds[idx_ind]->m_nbr.push_back(i.second);
		m_inds[idx_ind]->m_nbr.push_back(idx_ind);
	}
}