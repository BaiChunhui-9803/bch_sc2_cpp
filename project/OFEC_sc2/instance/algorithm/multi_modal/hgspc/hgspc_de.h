//Register HGSPC_DE "HGSPC-DE" MMOP,ConOP,SOP,GOP

#ifndef OFEC_HGSPC_DE_H
#define OFEC_HGSPC_DE_H

#include "../../template/framework/hgea/continuous/hgspc.h"

namespace ofec {
	class HGSPC_DE : public HGSPC {
	protected:
		Real m_F, m_CR;

		void initialize_() override;

		void reproduceSolution(size_t id_ind) override;
	};
}

#endif // !OFEC_HGSPC_DE_H
