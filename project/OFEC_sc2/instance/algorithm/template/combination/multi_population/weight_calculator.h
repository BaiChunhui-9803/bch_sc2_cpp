
#ifndef OFEC_WEIGHT_CALCULATOR_H
#define OFEC_WEIGHT_CALCULATOR_H
#include"../../../../../core/definition.h"
#include "../../../../../core/problem/encoding.h"
#include "../../../../../core/instance_manager.h"
#include<vector>
#include<list>

namespace ofec {
	class WeightCalculator {
		enum class mode { singleObjSigmoid , singleNormalize};
		mode m_mode = mode::singleObjSigmoid;
	public:

		void calWeight(int id_pro,const std::vector<SolBase*>& pop,
			std::vector<Real>& weight) {
			weight.resize(pop.size());
			if (pop.empty())return;
			switch (m_mode)
			{
			case WeightCalculator::mode::singleObjSigmoid:
			{
				Real m_preMemoryMaxObj = pop.front()->objective(0);
				Real m_preMemoryMinObj = pop.front()->objective(0);
				
				for (int i = 1; i < pop.size(); ++i) {
					if (m_preMemoryMaxObj < pop[i]->objective(0))
						m_preMemoryMaxObj = pop[i]->objective(0);
					if (m_preMemoryMinObj > pop[i]->objective(0))
						m_preMemoryMinObj = pop[i]->objective(0);
				}
				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1e-5;
				for (int i = 0; i < pop.size(); ++i) {
					if (GET_PRO(id_pro).optMode(0) == OptMode::kMinimize)
						weight[i] = (m_preMemoryMaxObj - pop[i]->objective(0) + 1e-5) / gap;
					else
						weight[i] = (pop[i]->objective(0) - m_preMemoryMinObj + 1e-5) / gap;
					weight[i] = 1 / (1 + exp(-weight[i]));
				}

			}
				break;
			case WeightCalculator::mode::singleNormalize:
			{
				Real m_preMemoryMaxObj = pop.front()->objective(0);
				Real m_preMemoryMinObj = pop.front()->objective(0);

				for (int i = 1; i < pop.size(); ++i) {
					if (m_preMemoryMaxObj < pop[i]->objective(0))
						m_preMemoryMaxObj = pop[i]->objective(0);
					if (m_preMemoryMinObj > pop[i]->objective(0))
						m_preMemoryMinObj = pop[i]->objective(0);
				}
				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1e-5;
				for (int i = 0; i < pop.size(); ++i) {
					if (GET_PRO(id_pro).optMode(0) == OptMode::kMinimize)
						weight[i] = (m_preMemoryMaxObj - pop[i]->objective(0) + 1e-5) / gap;
					else
						weight[i] = (pop[i]->objective(0) - m_preMemoryMinObj + 1e-5) / gap;
				}

			}
				break;
			default:
				break;
			}


		}
	};
}


#endif