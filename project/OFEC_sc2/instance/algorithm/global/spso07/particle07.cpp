#include "particle07.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/instance_manager.h"

namespace ofec {
	void Particle07::initVelocity(int id_pro, int id_rnd) {
		for (size_t j = 0; j < m_var.size(); j++) {
			auto &range = GET_CONOP(id_pro).range(j);
			m_vel[j] = (GET_RND(id_rnd).uniform.nextNonStd(range.first, range.second) - m_var[j]) / 2;
		}
	}

	void Particle07::nextVelocity(const Solution<> *lbest, Real w, Real c1, Real c2, int id_rnd) {
		if (lbest != &m_pbest) {
			for (size_t j = 0; j < m_var.size(); j++)
				m_vel[j] = w * m_vel[j]
				+ c1 * GET_RND(id_rnd).uniform.next() * (m_pbest.variable()[j] - m_var[j])
				+ c2 * GET_RND(id_rnd).uniform.next() * (lbest->variable()[j] - m_var[j]);
		}
		else {
			for (size_t j = 0; j < m_var.size(); j++)
				m_vel[j] = w * m_vel[j] + c1 * GET_RND(id_rnd).uniform.next() * (m_pbest.variable()[j] - m_var[j]);
		}
	}
}