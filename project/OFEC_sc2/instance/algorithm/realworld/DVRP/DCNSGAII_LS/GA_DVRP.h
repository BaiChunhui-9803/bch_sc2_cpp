#ifndef OFEC_GA_DVRP_H
#define OFEC_GA_DVRP_H
#include "../../../../problem/realworld/DVRP/dynamic_vrp.h"
#include "../../../../../core/algorithm/individual.h"
#include "../../../../../core/algorithm/population.h"
#include "../../../template/framework/dcmoea/dcmoea.h"

namespace ofec {
	class GA_DVRP {
	public:
		GA_DVRP();
		using DVRP_ind = std::unique_ptr<DCMOEA_ind<Individual<DVRP::routes>>>;
		void set_cr(Real cr) { m_cr = cr; }
		void set_mr(Real mr) { m_mr = mr; }
		void crossover(DCMOEA_ind<Individual<DVRP::routes>> &ind1, DCMOEA_ind<Individual<DVRP::routes>> &ind2, int id_rnd);
		void mutate(DCMOEA_ind<Individual<DVRP::routes>> &ind, int id_rnd);
	private:
		Real m_cr; // crossover probability
		Real m_mr; // mutation probability


	};






}








#endif // !OFEC_GA_DVRP_H

