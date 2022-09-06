#ifndef OFEC_LIPS_POP_H
#define OFEC_LIPS_POP_H

#include "../../template/classic/pso/particle.h"
#include "../../template/classic/pso/swarm.h"
#include <list>

namespace ofec {
	class SwarmLIP;
	class LI_particle : public Particle {
		friend SwarmLIP;
	private:
		std::vector<Real> m_pos;           // the mean position of neighborhood
		std::vector<Real> m_rdm;
		std::vector<int> m_nbr;
	public:
		LI_particle(size_t num_obj, size_t num_con, size_t size_var);
		void nextVelocityByWeight(Real w);
	};

	class SwarmLIP : public Swarm<LI_particle> {
	private:
		int m_M;            // the num of neighbors
		int m_max_evals;    // the maximum num of evaluations
		std::vector<std::list<std::pair<Real, size_t>>> m_dis;
		void setBestPos(int idx_ind, int id_pro, int id_rnd);
		void sortDistance(int idx_ind, int id_pro);
	public:
		SwarmLIP(size_t size_pop, int id_pro, int max_evals);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
	};
}

#endif // !OFEC_LIPS_POP_H

