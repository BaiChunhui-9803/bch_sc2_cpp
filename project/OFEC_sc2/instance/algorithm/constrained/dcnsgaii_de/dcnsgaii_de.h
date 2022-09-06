//Register DCNSGAII_DE "DCNSGAII-DE" ConOP,COP,MOP,SOP

#ifndef OFEC_DCNSGAII_DE_H
#define OFEC_DCNSGAII_DE_H

#include"../../.././../core/algorithm/population.h"
#include "../../template/classic/de/de_ind.h"
#include "../../template/framework/dcmoea/dcmoea.h"

namespace ofec {
	class DCNSGAII_DE_pop : public Population<DCMOEA_ind<IndDE>>, DCMOEA<DCMOEA_ind<IndDE>> {
	public:
		using typename Population<DCMOEA_ind<IndDE>>::IndType;
	public:
		DCNSGAII_DE_pop(size_t size_pop, int id_pro);
		int evolve(int id_pro, int id_alg, int id_rnd);
		void initialize(int id_pro, int id_alg, int id_rnd);
		void sort(int id_pro);
	protected:
		void select_next_parent_population(std::vector<IndType*>& pop, int id_pro);
		void reduce_radius(int id_pro);
		void caculate_nichecount(std::vector<IndType*>& pop, int id_pro);
		Dominance e_Pareto_compare(IndType* const& s1, IndType* const& s2, int id_pro);
		Dominance Pareto_compare(IndType* const& s1, IndType* const& s2, int id_pro);
	protected:
		Real m_r = 0, m_R, m_CR, m_F;
		std::vector<std::unique_ptr<IndType>> m_offspring;
		std::vector<std::unique_ptr<IndType>> m_temp_pop;
		RecombineDE m_recombine_strategy;
		std::vector<int> m_rand_seq;
		bool m_flag = false;
	};

	class DCNSGAII_DE : public Algorithm {
	protected:
		std::unique_ptr<DCNSGAII_DE_pop> m_pop;
		size_t m_pop_size;
	public:
		void initialize_();
		void run_();
		void initPop();
		void record();
	};
}



#endif // !OFEC_DCNSGAII_DE_H
