#include "particle11.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../utility/linear_algebra/vector.h"

namespace ofec {
	void Particle11::initVelocity(int id_pro, int id_rnd) {
		for (size_t j = 0; j < m_var.size(); j++) {
			auto& range = GET_CONOP(id_pro).range(j);
			m_vel[j] = GET_RND(id_rnd).uniform.nextNonStd(range.first - m_var[j], range.second - m_var[j]);
		}
	}

	void Particle11::clampVelocity(int id_pro, int id_rnd) {
		for (size_t j = 0; j < m_var.size(); j++) {
			auto& range = GET_CONOP(id_pro).range(j);
			if (m_var[j] > range.second) {
				m_var[j] = range.second;
				m_vel[j] *= -0.5;
			}
			else if (m_var[j] < range.first) {
				m_var[j] = range.first;
				m_vel[j] *= -0.5;
			}
		}
	}

	void Particle11::nextVelocity(const Solution<>* lbest, Real w, Real c1, Real c2, int id_rnd) {
		std::vector<Real> G(m_var.size());

		if (lbest != &m_pbest) {
			for (size_t j = 0; j < m_var.size(); j++)
				G[j] = m_var[j] + c1 * (m_pbest.variable()[j] + lbest->variable()[j] - 2 * m_var[j]) / 3;
		}
		else {
			for (size_t j = 0; j < m_var.size(); j++)
				G[j] = m_var[j] + c1 * (m_pbest.variable()[j] - m_var[j]) / 2;
		}

		Real dis = 0;
		for (size_t j = 0; j < m_var.size(); j++)
			dis += (G[j] - m_var[j]) * (G[j] - m_var[j]);
		dis = sqrt(dis);

		Vector v(m_var.size(), 0);
		v.randomizeInSphere(dis, &GET_RND(id_rnd).uniform);
		v += G;

		for (size_t j = 0; j < m_var.size(); j++)
			m_vel[j] = w * m_vel[j] + v[j] - m_var[j];
	}
}