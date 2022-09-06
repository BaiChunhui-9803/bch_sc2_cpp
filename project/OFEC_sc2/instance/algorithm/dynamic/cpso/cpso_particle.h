#ifndef CPSO_PARTICLE_H
#define CPSO_PARTICLE_H

#include "../../template/classic/pso/particle.h"
#include<vector>

namespace ofec {
    class CPSOParticle : public Particle
    {
    public:
        CPSOParticle(size_t num_obj, size_t num_con, size_t size_var);
        ~CPSOParticle();

        CPSOParticle(const CPSOParticle& other);
        CPSOParticle& operator = (const CPSOParticle& other);

        void initializeVmax(int id_pro, int id_rnd);
        void nextVelocity(const Solution<>* lbest, Real w, Real c1, Real c2, int id_rnd) override;
        void initVelocity(int id_pro, int id_rnd) override;
        void clampVelocity(int id_pro, int id_rnd);

    protected:
        struct VMax {
            double m_min, m_max;
            VMax() :m_min(0), m_max(0) {}
        };
        std::vector<VMax> m_vmax;
    };
}

#endif // CPSO_PARTICLE_H
