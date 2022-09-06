#ifndef OFEC_DE_NRAND_1_POP_H
#define OFEC_DE_NRAND_1_POP_H

#include "../../template/classic/de/de_pop.h"

namespace ofec {
	class PopNRand1DE : public PopDE<> {
	public:
		PopNRand1DE() = default;
		PopNRand1DE(const PopNRand1DE &rhs) = default;
		PopNRand1DE(PopNRand1DE &&rhs) noexcept = default;
		PopNRand1DE(size_t size_pop, int id_pro);
		int evolve(int id_pro, int id_alg, int id_rnd) override;
	};
}
#endif // OFEC_DE_NRAND_1_POP_H
