#ifndef OFEC_GL_ADAPTOR_CONT_H
#define OFEC_GL_ADAPTOR_CONT_H

#include "../../template/framework/gl/gl_adaptor.h"
#include "../../../../core/algorithm/individual.h"
#include <algorithm>

namespace ofec {
	class AdaptorContGL : public AdaptorGL<Individual<>> {
	public:
		AdaptorContGL(Real alpha, size_t num_dim, size_t size_pop);
		void updateProbability(int id_pro,
			std::vector<std::unique_ptr<Individual<>>>& pop, 
			const std::vector<Real>& weight, 
			const std::vector<int>* index = nullptr) override;
		void createSolution(int id_pro, int id_rnd,
			std::vector<std::unique_ptr<Individual<>>>& pop, 
			std::vector<Individual<>>& offspring) override;
		int updateSolution(int id_pro, int id_alg,
			std::vector<std::unique_ptr<Individual<>>>& pop, 
			std::vector<Individual<>>& offspring, int& num_improve) override;
		void updateStep(int id_pro, std::vector<std::unique_ptr<Individual<>>>& pop);
	protected:
		void accumlateProbability();
		void localSearch(int id_pro, int id_rnd, size_t i, 
			std::vector<std::unique_ptr<Individual<>>>& pop, 
			std::vector<Individual<>>& offspring);
		void globalSearch(int id_pro, int id_rnd, size_t i, 
			std::vector<std::unique_ptr<Individual<>>>& pop, 
			std::vector<Individual<>>& offspring);
	protected:
		int m_num;
		struct Infor {
			Real val;
			std::vector<int> idx;
		};
		std::vector<std::vector<Infor> > m_proba;
		std::vector<std::vector<Real> >m_acc_proba;
		struct Limit {
			std::pair<Real, Real>boundary;
			Real step, range;
			std::vector<Real> as;
		};
		std::vector<Limit> m_limit/*, m_initialRange*/;
		std::vector<std::vector<int>> m_pos;
	};
}

#endif // !OFEC_GL_ADAPTOR_CONT_H

