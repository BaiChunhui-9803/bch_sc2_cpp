#include "particle.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
	Particle::Particle(size_t num_obj, size_t num_con, size_t size_var) :
	        Individual(num_obj, num_con, size_var),
	        m_pbest(num_obj, num_con, size_var),
	        m_vel(size_var) {}

	Particle::Particle(const Solution<>& rhs) : 
		Individual(rhs), 
		m_pbest(rhs), 
		m_vel(rhs.variable().size()) {}

	Particle& Particle::operator=(const Solution<>& other) {
		Solution<>::operator=(other);
		m_pbest = other;
		return *this;
	}

	Real Particle::speed() const {
		Real ve = 0;
		for (size_t i = 0; i< variable().size(); i++)
			ve += m_vel[i] * m_vel[i];
		return sqrt(ve);
	}

	void Particle::initVelocity(int id_pro, int id_rnd) {
		for (size_t i = 0; i< variable().size(); i++) {
			auto& range = GET_CONOP(id_pro).range(i);
			m_vel[i] = (range.second - range.first) * (-0.5 + GET_RND(id_rnd).uniform.next()) / 2;
		}
	}

	void Particle::initVelocity(const std::vector<std::pair<Real, Real>> &vrange, int id_rnd) {
		for (size_t i = 0; i<variable().size(); i++) {	
			m_vel[i] = (vrange[i].second - vrange[i].first) * (-0.5 + GET_RND(id_rnd).uniform.next()) / 2;
		}
	}

	void Particle::initVelocity(Real min, Real max, int id_rnd) {
		for (int i = 0; i < variable().size(); i++) {
			m_vel[i] = GET_RND(id_rnd).uniform.nextNonStd(min, max);
		}
	}

	void Particle::nextVelocity(const Solution<> *lbest, Real w, Real c1, Real c2, int id_rnd) {
		for (size_t j = 0; j<variable().size(); j++) {
			m_vel[j] = w * m_vel[j]
				+ c1 * GET_RND(id_rnd).uniform.next() * (m_pbest.variable()[j] - variable()[j])
				+ c2 * GET_RND(id_rnd).uniform.next() * (lbest->variable()[j] - variable()[j]);
		}
	}

	void Particle::clampVelocity(int id_pro, int id_rnd) {
		for (size_t j = 0; j<variable().size(); j++) {	
			auto& range = GET_CONOP(id_pro).range(j);
			if (variable()[j]>range.second) {
				variable()[j] = range.second;
				m_vel[j] = 0;
			}
			else if (variable()[j]<range.first) {
				variable()[j] = range.first;
				m_vel[j] = 0;
			}
		}
	}
	
	void Particle::move() {
		for (size_t j = 0; j<variable().size(); j++) {			
			variable()[j] += m_vel[j];
		}
	}
	
	void Particle::resizeVar(size_t n) {
		variable().resize(n);
		m_pbest.variable().resize(n);
		m_vel.resize(n);
	}
}
