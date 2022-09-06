#ifndef SPSOSWARM_H
#define SPSOSWARM_H

#include "spso_particle.h"
#include "../../template/classic/pso/swarm.h"
#include"../../../../core/problem/continuous/continuous.h"
#include <memory>

namespace ofec {
    class SPSOSwarm : public Swarm<SPSOParticle> {
    public:
        SPSOSwarm(size_t pop, int id_pro);
        void initialize(int id_pro, int id_rnd);
        void findpopSeed();
        int evolve(int id_pro, int id_alg, int id_rnd) override;
        int best_idx(int id_pro);
        void setNeighborhood(int id_rnd) override;
        //SPSOParticle getpopSeed() { return m_seed; }
    protected:
        //SPSOParticle m_seed;
    };
}

#endif
