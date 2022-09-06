//Register ContGL "GLC" ConOP,GOP,MMOP,SOP

#ifndef OFEC_GL_CONT_H
#define OFEC_GL_CONT_H
#include "gl_cont_pop.h"
#include "../../../../core/algorithm/individual.h"

namespace ofec {
	class ContGL : public Algorithm {
		using UpdateScheme = PopContGL::UpdateScheme;
	protected:
		void initialize_() override;
		void run_() override;
	public:
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif
	protected:
		size_t m_pop_size;
		UpdateScheme m_update_scheme;
		Real m_alpha, m_beta, m_gamma;
		PopContGL m_pop;
	};
}


#endif // !OFEC_GL_CONT_H

