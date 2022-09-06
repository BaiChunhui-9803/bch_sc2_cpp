#ifndef AMP_POP_H
#define AMP_POP_H

#include "../../../../../core/algorithm/multi_population.h"
#include "../../../../../core/global.h"
#include <algorithm> 
#include <vector>

namespace ofec {
	template<typename>  class AMP;

	template <typename TPop>
	class PopAMP : public TPop {
	public:
		template<typename> friend class AMP;
		using IndType = typename TPop::IndType;

	protected:
		bool m_stagnant = false;
		Real m_cur_radius = 0;
		Real m_initial_radius = 0;
		int m_count_radius_decre = 0;

	//	int mc_max_idxs, mc_min_idxs;
	public:
		PopAMP(size_t pop_size, int id_pro) : TPop(pop_size, id_pro) {}
		void setInitialRadius(Real r) {
			m_initial_radius = r;
		}
		Real initialRadius() const{
			return m_initial_radius;
		}
		Real curRadius() const {
			return m_cur_radius;
		}
		bool isStagnant() const {
			return m_stagnant;;
		}
		// r1 domiante r2
		Real updateRadius(Real  r1, Real r2) {
			if (r1 > r2) {
				r1 -= (r1 - r2)*r2 / r1;
			}
			else {
				r1 += (r2 - r1)*r1 / r2;
			}

			return r1;
		}

		Real avgDisTo(const IndType& indi, int id_pro) {
			if (this->m_inds.empty()) {
				return 0;
			}
			else {
				Real avg_dis(0);
				for (auto &it : this->m_inds) {
					avg_dis += indi.variableDistance(*it, id_pro);
				}
				return avg_dis / static_cast<Real>(this->m_inds.size());
			}
		}

		virtual void udpateCurRadius(int id_pro) = 0;

		virtual void initRadius(int id_pro) {
			udpateCurRadius(id_pro);
			m_initial_radius = m_cur_radius;
		}

		virtual bool judgeConverged() { return false; }

		int evolve(int id_pro, int id_alg, int id_rnd) override {
			Real before_radius(m_cur_radius);
			int rf = TPop::evolve(id_pro, id_alg, id_rnd);
			udpateCurRadius(id_pro);
			if (m_cur_radius / (before_radius + 0.00001) < 0.9) m_count_radius_decre = 0;
			else m_count_radius_decre++;
			return rf;
		}
	};

}

#endif