//Register CEP "CEP" ConOP,GOP,SOP

#ifndef OFEC_CEP_H
#define OFEC_CEP_H

#include "../../template/classic/ep/ep_pop.h"
#include "../../../../core/algorithm/algorithm.h"

namespace ofec {
	class CEP : public Algorithm {   
	protected:
		void initialize_() override;
		void run_() override;

	public:
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	protected:
		Real m_tau, m_tau_prime;
		size_t m_q, m_pop_size;
		PopEP<> m_pop;
	};
}

#endif // !OFEC_CEP_H
