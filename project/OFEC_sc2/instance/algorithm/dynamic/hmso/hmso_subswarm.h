#ifndef HMSOSWARM_H
#define HMSOSWARM_H

#include "../../template/classic/pso/particle.h"
#include "../../template/classic/pso/swarm.h"
#include"../../../../core/problem/continuous/continuous.h"
#include <memory>

namespace ofec {
    class HmSOSwarm : public Swarm<Particle> {
    public:
        enum SwarmType { Parent, Child };
        HmSOSwarm(size_t pop, int id_pro, SwarmType t);
        //void initialize();
        void setswarmType(SwarmType t) { m_swarmtype = t; }
        SwarmType getswarmType() { return m_swarmtype; }

        void setexcelFlag(bool flag) { m_excelflag = flag; }
        bool getexcelFlag() { return m_excelflag; }
        void sethiberFlag(bool flag) { m_hiberflag = flag; }
        bool gethiberFlag() { return m_hiberflag; }
        bool gethiberFlag() const{ return m_hiberflag; }
        int evolve(int id_pro, int id_alg, int id_rnd);
        void setNeighborhood(int id_rnd) override;
        int best_idx(int id_pro) {
            int l = 0;
            for (int i = 0; i < this->m_inds.size(); ++i) {
                if (this->m_inds[i]->pbest().dominate(this->m_inds[l]->pbest(), id_pro)) {
                    l = i;
                }
            }
            return l;
        }
        void computeCenter(int id_pro);
        void updateCurRadius(int id_pro);
        Real getCurRadius() { return m_radius; }

    protected:
        SwarmType m_swarmtype;
        bool m_excelflag = false;
        bool m_hiberflag = false;
        Real m_radius;
        std::unique_ptr<Solution<>> m_center;
    };
}


#endif
