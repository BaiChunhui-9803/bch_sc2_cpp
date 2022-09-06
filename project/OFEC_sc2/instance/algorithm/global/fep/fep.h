//Register FEP "FEP" ConOP,GOP,SOP

#ifndef OFEC_FEP_H
#define OFEC_FEP_H

#include "../../template/classic/ep/ep_pop.h"
#include "../../../../core/algorithm/algorithm.h"

namespace ofec {
	class PopFEP : public PopEP<> {
	protected:
		void mutate(int id_rnd, int id_pro) override;
	};

	class FEP : public Algorithm {
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
		PopFEP m_pop;
	};
}

#endif // !OFEC_FEP_H
