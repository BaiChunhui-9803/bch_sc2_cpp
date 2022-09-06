#ifndef CPSOSWARM_H
#define CPSOSWARM_H

#include "cpso_particle.h"
#include "../../template/classic/pso/swarm.h"
#include"../../../../core/problem/continuous/continuous.h"
#include"../../../problem/continuous/dynamic/uncertianty_continuous.h"
#include <memory>

namespace ofec {
    using templateParticle = CPSOParticle;
    class CPSOSwarm : public Swarm<templateParticle> {
    public:
        CPSOSwarm(size_t pop, int id_pro);
        void initializeParameters(int id_pro, int id_alg, int id_rnd);
        //CPSOSwarm()
        void initialize(int id_pro, int id_rnd) override;
        void initializeRadius();

        int localSearch(int id_pro, int id_alg, int id_rnd, int pop_size);

        void setNeighborhood(int id_rnd) override;
        void setConverge(bool flag) { m_converge_flag = flag; }
        bool getConverge() const { return m_converge_flag; }

        int learnGbest(const std::unique_ptr<templateParticle>& pi, std::unique_ptr<templateParticle>& gbest, int id_pro, int id_alg) const;

        int best_idx(int id_pro);

        void computeCenter(int id_pro);

        void updateCurRadius(int id_pro);

        void checkOverCrowd(size_t max_sub_size, int id_pro, int id_rnd);

        void sortPbest(int id_pro);

        void merge(CPSOSwarm& pop, int id_pro, int id_rnd);

        Real getInitCurRadius() const { return m_initradius; }
        Real getCurRadius() const { return m_radius; }

        std::unique_ptr<Solution<>> & center() { return m_center; }

    protected:
        double m_W, m_maxW, m_minW;                // inertia weight
        size_t m_evals, m_u;
        Real m_radius;
        Real m_initradius;

        bool m_converge_flag;
        std::unique_ptr<Solution<>> m_center;
    };
}

#endif
