#ifndef FTMPSOSWARM_H
#define FTMPSOSWARM_H

#include "../../template/classic/pso/particle.h"
#include "../../template/classic/pso/swarm.h"
#include"../../../../core/problem/continuous/continuous.h"
#include <memory>

namespace ofec {
    class FTMPSOSwarm : public Swarm<Particle> {
    public:
        enum class SwarmType { FINDER, TRACKER };
        FTMPSOSwarm(size_t pop, int id_pro, SwarmType t);
        void initialize(int id_pro, int id_rnd);
        void setSwarmType(SwarmType t) { m_swarmtype = t; }
        SwarmType getSwarmType() { return m_swarmtype; }

        void setexcelFlag(bool flag) { m_excelFlag = flag; }
        bool getexcelFlag() { return m_excelFlag; }
        void sethiberFlag(bool flag) { m_hiberFlag = flag; }
        bool gethiberFlag() { return m_hiberFlag; }
        bool gethiberFlag() const { return m_hiberFlag; }
        int evolve(int id_pro, int id_alg, int id_rnd);
        void setNeighborhood(int id_rnd) override;
        int best_idx(int id_pro);
        void computeCenter(int id_pro);
        void updateCurRadius(int id_pro);
        Real getCurRadius() { return m_radius; }

    protected:
        SwarmType m_swarmtype;
        bool m_excelFlag = false;
        bool m_hiberFlag = false;
        Real m_radius;
        std::unique_ptr<Solution<>> m_center;
        std::unique_ptr<Solution<>> m_xR;
    };
}

#endif
