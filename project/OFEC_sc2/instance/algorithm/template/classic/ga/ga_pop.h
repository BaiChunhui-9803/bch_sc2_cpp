#ifndef OFEC_GA_POP_H
#define OFEC_GA_POP_H

#include "../../../../../core/algorithm/population.h"
#include "../../../../../core/algorithm/individual.h"
#include <numeric>

namespace ofec {
	template<typename TInd>
	class PopGA : public Population<TInd> {
	private:
		std::vector<size_t> m_rand_seq; // Random sequence of the population for tournamentSelection()
	protected:
		Real m_cr; // crossover probability
		Real m_mr; // mutation probability

	public:
		template<typename ... Args>
		PopGA(size_t size_pop, Args&& ...args) :
			Population<TInd>(size_pop, std::forward<Args>(args)...),
			m_cr(0.5),
			m_mr(0) {}
		void setCR(Real cr) { m_cr = cr; }
		void setMR(Real mr) { m_mr = mr; }
		virtual void crossover(size_t idx_parent1, size_t idx_parent2, TInd &child1, TInd &child2, int id_pro, int id_rnd) = 0;
		virtual void mutate(TInd &ind, int id_pro, int id_rnd) = 0;
		size_t tournamentSelection(int id_pro, int id_rnd, size_t tournament_size = 2);
	};

	template<typename TInd>
	size_t PopGA<TInd>::tournamentSelection(int id_pro, int id_rnd, size_t tournament_size) {
		if (m_rand_seq.size() != this->m_inds.size()) {
			m_rand_seq.resize(this->m_inds.size());
			std::iota(m_rand_seq.begin(), m_rand_seq.end(), 0);
		}
		GET_RND(id_rnd).uniform.shuffle(m_rand_seq.begin(), m_rand_seq.end());
		size_t idx_best = m_rand_seq[0];
		for (size_t k = 1; k < tournament_size; ++k)
			if (this->m_inds[m_rand_seq[k]]->dominate(*this->m_inds[idx_best], id_pro))
				idx_best = m_rand_seq[k];
		return idx_best;
	}
}

#endif // !OFEC_GA_POP_H

