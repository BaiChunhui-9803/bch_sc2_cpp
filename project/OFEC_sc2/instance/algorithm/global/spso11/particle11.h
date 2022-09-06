#ifndef OFEC_PARTICLE11_H
#define OFEC_PARTICLE11_H

#include "../../template/classic/pso/particle.h"

namespace ofec {
	class Particle11 final : public Particle {
	public:
		Particle11(size_t num_obj, size_t num_con, size_t size_var) : Particle(num_obj, num_con, size_var) {}
		Particle11(const Solution<>& rhs) : Particle(rhs) {}
		void initVelocity(int id_pro, int id_rnd) override;
		void nextVelocity(const Solution<>* lbest, Real w, Real c1, Real c2, int id_rnd) override;
		void clampVelocity(int id_pro, int id_rnd) override;
	};
}

#endif // !OFEC_PARTICLE11_H
