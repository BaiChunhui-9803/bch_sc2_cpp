#ifndef AMP_POP_CONTINUOUS_H
#define AMP_POP_CONTINUOUS_H

#include "../../../../../../core/problem/continuous/continuous.h"
#include "../../../../../../core/algorithm/multi_population.h"
#include "../../../../../../core/global.h"
#include "hibernated_area.h"
#include <vector>
#include "../amp_pop.h"

namespace ofec {
	template <typename TPop>
	class PopContAMP : public PopAMP<TPop> {
	public:
		using IndType = typename TPop::IndType;

	protected:
		Real m_converg_threshold = 0.0001;
		Real m_converg_factor = 0.005;
		IndType m_center;

	public:
		PopContAMP(size_t pop_size, int id_pro) : 
			PopAMP<TPop>(pop_size, id_pro), 
			m_center(GET_PRO(id_pro).numObjectives(), 
				GET_PRO(id_pro).numConstraints(),
				GET_CONOP(id_pro).numVariables()) {}

		int evolve(int id_pro, int id_alg, int id_rnd) override {
			int rf = PopAMP<TPop>::evolve(id_pro, id_alg, id_rnd);
			this->updateBest(id_pro);
			if (rf)return rf;
			std::vector<IndType> new_inds;
			for (auto& best : this->m_best) {
				new_inds.push_back(*best);
				if (this->m_stagnant) {
					new_inds.back().cauchyMove(id_pro, id_rnd);
				}
				else {
					new_inds.back().brwonianMove(id_pro, id_rnd, this->m_cur_radius);
				}
			}
			for (auto& new_ind : new_inds) {
				rf = new_ind.evaluate(id_pro, id_alg);
				this->updateBest(new_ind, id_pro);
				if (rf )return rf;
			}
			return rf;
		}

		void setParameters(Real convergThreshold, Real convFactor) {
			m_converg_threshold = convergThreshold;
			m_converg_factor = convFactor;
		}

		virtual void degradeExploredAreas(HibernatedArea& h, int id_pro, int id_alg, int id_rnd) {
			for (auto& it : this->m_best) {
				h.derateFitness(*it, id_pro, id_alg, id_rnd);
			}
			for (auto& it : this->m_inds) {
				h.derateFitness(*it, id_pro, id_alg, id_rnd);
				// for pso
				//h.derateFitness(it->pbest());
			}
		}

		void updateStagnant(double avgRadius, int id_pro) {
			if (this->m_count_radius_decre >= 1. * this->size() && 
				this->m_cur_radius >= avgRadius && 
				this->m_cur_radius > m_converg_factor * GET_CONOP(id_pro).domainArea() || 
				this->m_count_radius_decre >= 10 * this->size())
				this->m_stagnant = true;
			else
				this->m_stagnant = false;
		}

		bool judgeConverged() override {
			return this->m_cur_radius <= m_converg_threshold;
		}

		void udpateCurRadius(int id_pro) override {
			calculateCenter();
			this->m_cur_radius = this->avgDisTo(m_center, id_pro);
		}

		const IndType& center() const {
			return m_center;
		}

		void calculateCenter() {
			if (this->m_inds.empty())return;
			m_center = *this->m_inds.front();
			size_t var_size(m_center.variable().size());
			for (decltype(this->m_inds.size()) i(1); i < this->m_inds.size(); ++i) {
				for (int j(0); j < var_size; ++j) {
					m_center.variable()[j] += this->m_inds[i]->variable()[j];
				}
			}
			for (int j(0); j < var_size; ++j) {
				m_center.variable()[j] /= this->m_inds.size();
			}
		}
	};
}

#endif