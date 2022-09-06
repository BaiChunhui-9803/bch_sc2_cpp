#ifndef MP_COM_POP_H
#define MP_COM_POP_H

#include "../../../../../core/algorithm/population.h"
#include "../../../../../core/global.h"

namespace ofec {
	template<typename TPop>
	class MP_COM_Pop : public TPop {
	public:
		using IndividualType = typename TPop::IndividualType;
	protected:
		std::list<std::unique_ptr<IndividualType>> m_discard_indis;
	public:
		virtual void addSol(int idx,const IndividualType& indi) {
			if (idx != -1)
				*this->m_inds[idx] = indi;
		}
		virtual void addSol(const IndividualType& indi) {
			this->m_inds.emplace_back(new IndividualType(indi));
		}
		virtual void setRadius(Real radius) {}
		std::list<std::unique_ptr<IndividualType>>& getDiscardIndis() {
			return m_discard_indis;
		}

		virtual void mergePop(MP_COM_Pop& pop) = 0;
		virtual void distance(const MP_COM_Pop& pop) = 0;

		
		

		virtual void initPopAround(int id_pro,int id_alg,const IndividualType& center, Real radius, size_t* popSize=nullptr) = 0;
	protected:
		Real m_innerRadius = 1.0;
		Real m_outerRadius = 0.0;

	};
}


#endif