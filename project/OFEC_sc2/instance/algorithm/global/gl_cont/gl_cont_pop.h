#ifndef OFEC_GL_CONT_POP_H
#define OFEC_GL_CONT_POP_H

#include "../../template/framework/gl/gl_pop.h"
#include "../../../../core/algorithm/individual.h"

namespace ofec {
	class PopContGL : public PopGL<Individual<>> {
	public:
		void resize(size_t size_pop, int id_pro);
		void initialize(int id_pro, int id_rnd) override;
		int evolve(int id_pro, int id_alg, int id_rnd) override;
	protected:
		void initializeCurpop();
	};
}


#endif // !OFEC_GL_CONT_POP_H

