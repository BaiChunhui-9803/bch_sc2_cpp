#ifndef OFEC_HGSPC_H
#define OFEC_HGSPC_H

#include "../hgea.h"

namespace ofec {
	class HGSPC : public HGEA {
	protected:
		size_t m_num_vars, m_num_ssps;

		void initialize_() override;
#ifdef OFEC_DEMO
		void updateBuffer() override;
#endif

		void initializeSolution(size_t id_ind) override;
	};
}

#endif // !OFEC_HGSPC_H
