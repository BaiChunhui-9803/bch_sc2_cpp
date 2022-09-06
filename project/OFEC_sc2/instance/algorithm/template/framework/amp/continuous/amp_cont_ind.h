#ifndef OFEC_AMP_INDIVIDUAL_CONTINUOUS_H
#define OFEC_AMP_INDIVIDUAL_CONTINUOUS_H

#include "../../../../../../core/instance_manager.h"
#include "../../../../../../core/problem/continuous/continuous.h"
#include "../../../../../../core/problem/solution.h"

namespace ofec {
	template<class IndType>
	class IndContAMP : public IndType {
	protected:
		// whether the individual fall into explored area
		bool m_explored = false;
		std::vector<Real> m_real_obj;

	public:
		IndContAMP(size_t size_obj, size_t size_con, size_t size_var) : IndType(size_obj, size_con, size_var) {}
		IndContAMP(const Solution<> &rhs) : IndType(rhs) {}
		void brwonianMove(int id_pro, int id_rnd, double radius) {
			for (size_t j = 0; j < this->m_var.size(); j++) {
                this->m_var[j] += GET_RND(id_rnd).normal.nextNonStd(0, radius);
			}
			GET_CONOP(id_pro).validateSolution(*this, Validation::kSetToBound, id_rnd);
		}
		void cauchyMove(int id_pro, int id_rnd, double radius=-1) {
			auto& domain = GET_CONOP(id_pro).domain();
			for (size_t i = 0; i < this->m_var.size(); i++) {
				if (radius < 0) {
                    this->m_var[i] += GET_RND(id_rnd).cauchy.nextNonStd(0, (domain[i].limit.second - domain[i].limit.first) / 2);
				}
				else {
                    this->m_var[i] += GET_RND(id_rnd).cauchy.nextNonStd(0, radius);
				}
			}
			GET_CONOP(id_pro).validateSolution(*this, Validation::kSetToBound, id_rnd);
		}
	};

}

#endif // ! OFEC_AMP_INDIVIDUAL_CONTINUOUS_H
