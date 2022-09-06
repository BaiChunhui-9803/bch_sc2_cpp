#ifndef OFEC_PARTICLE07_H
#define OFEC_PARTICLE07_H

#include "../../template/classic/pso/particle.h"

namespace ofec {
	class Particle07 final : public Particle {
	public:
		Particle07(size_t num_obj, size_t num_con, size_t size_var) : Particle(num_obj, num_con, size_var) {}
		Particle07(const Solution<> &rhs) : Particle(rhs) {}
		void initVelocity(int id_pro, int id_rnd) override;
		void nextVelocity(const Solution<> *lbest, Real w, Real c1, Real c2, int id_rnd) override;
	};
}

#endif // !OFEC_PARTICLE07_H
