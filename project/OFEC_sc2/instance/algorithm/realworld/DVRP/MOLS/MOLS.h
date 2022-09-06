//Register MOLS "MOLS" DVRP

#ifndef OFEC_MOLS_H
#define OFEC_MOLS_H

#include "../../../../../core/algorithm/population.h"
#include "../../../DCO/DCMOEA.h"
#include "../INT_DVRP_POP/INIT_DVRP_POP.h"

namespace ofec {
	class MOLS_pop : public population<DCMOEA_ind<individual<DVRP::routes, Real>>>, DCMOEA<DCMOEA_ind<individual<DVRP::routes, Real>>> {
	public:
		using population<DCMOEA_ind<individual<DVRP::routes, Real>>>::individual_type;
	public:
		MOLS_pop(size_t size_pop);
		void initialize();
		individual_type &select_sol(size_t obj_id);
		int evolve();
		//MOLS
		void LS(individual_type &offspring, size_t obj_id);
		void updateA(individual_type offspring);
		bool feasible_check(individual_type &offspring);
		bool check_order(individual_type &offspring);
		int pop_size() {return m_pop_size;}
		void sort();
		const size_t get_num_vio_obj() { return m_num_vio_obj; }
		void Online();
		void set_bestInd(const DCMOEA_ind<individual<DVRP::routes, Real>> &best_ind) { m_best_ind_Offline = best_ind; }
		const DCMOEA_ind<individual<DVRP::routes, Real>> &get_bestInd() { return m_best_ind_Online; }
		
	protected:		
		dominationship e_Pareto_compare(individual_type* const&s1, individual_type* const&s2);
		dominationship Pareto_compare(individual_type* const&s1, individual_type* const&s2);
		
	protected:
		std::vector< std::unique_ptr<individual_type>> m_archive;
		int m_maxDepth = 100;
		int m_pop_size = 100;

		INIT_DVRP_POP m_ahc_tsp;
		size_t m_num_vio_obj = 1;
		
		//Online
		individual_type m_best_ind_Online;
		individual_type m_best_ind_Offline;
		std::vector<std::unique_ptr<individual_type>> m_best_inds;
		bool m_insert_success = false;
		bool m_evo_success_Online = false;
		int m_cnt_fail_Online = 0;
		int m_OnlineIters = 2000;
		std::vector<int> m_score_Online;
		std::vector<Real> m_p_select_Online;
		std::vector<Real> m_cp_Online;//cumulative probability
		bool m_convergence_Online = false;
		int m_num_operators_Online = 3;
		void construct_path_Online(individual_type & ind, size_t i);
		void updateSeqOnline(individual_type & ind, size_t i);
		void updateSeqOnline_inserted(individual_type & ind, size_t i, std::pair<int,int>);
		void Online_ALS(individual_type & ind, size_t i);
		void ls_Rswap2_Online(individual_type &ind, size_t i);
		void ls_maxDT_Online(individual_type &ind, size_t i);
		void ls_2opt_Online(individual_type &ind, size_t i);
	};

	class MOLS : public algorithm {
	protected:
		MOLS_pop m_pop;
	public:
		MOLS(param_map& v);
		void initialize();
		void run_();
		void record(); 
#ifdef OFEC_DEMO
		void updateBuffer() override{}
#endif
	};
}



#endif // !OFEC_MOLS_H
