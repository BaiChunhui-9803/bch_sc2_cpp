//Register ConSA "Con-SA" ConOP,GOP,SOP,MMOP

#include "../../template/classic/sa/simulated_annealing.h"
#include "../../../../core/problem/solution.h"

namespace ofec {
	class ConSA : public SimulatedAnnealing<Solution<>> {
	protected:
		void initSol() override;
		void neighbour(const Solution<>& s, Solution<>& s_new) override;
	};
}