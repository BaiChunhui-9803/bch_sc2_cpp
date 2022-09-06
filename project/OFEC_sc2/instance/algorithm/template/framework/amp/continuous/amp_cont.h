#ifndef AMP_CONTINUOUS_H
#define AMP_CONTINUOUS_H

#include "../amp.h"
#include "../../../../../../core/problem/continuous/continuous.h"
#include "amp_cont_pop.h"
#include "hibernated_area.h"

namespace ofec {
	template <typename TPop>
	class ContAMP : public AMP<PopContAMP<TPop>> {
	public:
		using typename AMP<PopContAMP<TPop>>::IndType;
	protected:
		Real m_conv_thresh = 0.0001;
		Real m_conv_factor = 0.005;
		HibernatedArea m_hibernated_area;
		
		void initIndividuals(int individual_num, int id_pro, int id_alg, int id_rnd) override {
			if (this->m_new_indis.size() > individual_num) {
				this->m_new_indis.resize(individual_num);
			}
			else {
				int originSize = this->m_new_indis.size();
				this->m_new_indis.resize(individual_num);
				size_t num_objs = GET_PRO(id_pro).numObjectives();
				size_t num_cons = GET_PRO(id_pro).numConstraints();
				size_t num_vars = GET_CONOP(id_pro).numVariables();
				for (int i(originSize); i < individual_num; ++i) {
					this->m_new_indis[i].reset(new IndType(num_objs, num_cons, num_vars));
					this->m_new_indis[i]->initialize(i, id_pro, id_rnd);
					this->m_new_indis[i]->evaluate(id_pro, id_alg);
				}
			}
		}

		void updateStagnateState(int id_pro) override;
		void removeRedundentHiber(int id_pro) override;
		bool checkIncreaseDiv(int id_pro) override;
		void degradeExploredAreas(int id_pro, int id_alg, int id_rnd) override;
		bool judgeOverlapping(const PopContAMP<TPop>& p1, const PopContAMP<TPop>& p2, int id_pro) override {
			Real center_dis(GET_PRO(id_pro).variableDistance(p1.center(), p2.center()));
			Real dis(0);
			if (center_dis < p1.initialRadius() && center_dis < p1.initialRadius()) {
				int c1(0), c2(0);
				for (size_t k = 0; k < p2.size(); k++) {
					dis = GET_PRO(id_pro).variableDistance(p1.center(), p2[k]);
					if (dis < p1.initialRadius()) c1++;
				}

				for (size_t k = 0; k < p1.size(); k++) {
					dis = GET_PRO(id_pro).variableDistance(p2.center(), p1[k]);
					if (dis < p2.initialRadius()) c2++;
				}
				return c1 > 0 && c2 > 0;
			}
			else
				return false;
		}

		void hibernatePop(PopContAMP<TPop>& cur_pop, int id_pro, int id_alg, int id_rnd) override {
			AMP<PopType>::hibernatePop(cur_pop, id_pro, id_alg, id_rnd);
			int idx(0);
			Real minDis(0);
			for (auto& it : cur_pop.best()) {
				m_hibernated_area.get_nearest_optimum(*it, idx, minDis, id_pro);
				if (minDis > GET_CONOP(id_pro).variableAccuracy()) {
					m_hibernated_area.add_optimum(*it, id_pro, id_alg, id_rnd);
				}
			}
		}
	
	public:
		ContAMP(size_t size_pop);
	};

	template<typename TPop>
	void ContAMP<TPop>::updateStagnateState(int id_pro){
		for (size_t i = 0; i < this->m_pops.size(); ++i) {
			if (this->m_pops[i]->isActive()) {
                this->m_pops[i]->updateStagnant(this->m_avgRadius, id_pro);
			}
		}
	}

	template<typename TPop>
	void ContAMP<TPop>::removeRedundentHiber(int id_pro) {
		int idx(0);
		Real minDis(0);
		for (auto iter = this->m_pops.begin(); iter != this->m_pops.end();) {
			if (!(*iter)->isActive()) {
				m_hibernated_area.get_nearest_optimum(*(*iter)->best(id_pro).front(), idx, minDis, id_pro);
				if (minDis < GET_CONOP(id_pro).variableAccuracy())
					iter = this->m_pops.erase(iter);
				else
					iter++;
			}
			else
				iter++;
		}
	}

	template<typename TPop>
	bool ContAMP<TPop>::checkIncreaseDiv(int id_pro) {
		if (this->m_pops.size() == 0)
			return true;

		Real avgRadius = 0;
		int count = 0;
		for (unsigned k = 0; k < this->m_pops.size(); k++) {
			if (this->m_pops[k]->isActive() && !this->m_pops[k]->isStagnant()) {
				avgRadius += this->m_pops[k]->curRadius();
				count++;
			}
		}
		if (count > 0)	avgRadius /= count;
		else return true;
		if (avgRadius <= this->m_conv_factor * GET_CONOP(id_pro).domainArea()) {//
			return true;
		}
		else return false;
	}

	template<typename TPop>
	void ContAMP<TPop>::degradeExploredAreas(int id_pro, int id_alg, int id_rnd) {
		for (int i(0); i < this->m_pops.size(); ++i) {
			this->m_pops[i]->degradeExploredAreas(m_hibernated_area, id_pro, id_alg, id_rnd);
		}
	}

	template<typename TPop>
	ContAMP<TPop>::ContAMP(size_t size_pop) :
		AMP<PopContAMP<TPop>>(size_pop) {}
}

#endif