#include "DynDE_subpopulation.h"
namespace ofec {
	SubPopDynDE::SubPopDynDE(size_t size, int id_pro) : 
		PopDE<IndDynDE>(size, id_pro), 
		m_num_normal(5), 
		m_num_brownian(5), 
		m_num_quantum(5),
		m_r_cloud(1.0), 
		m_sigma(0.2) 
	{
		// set m_Rcloud to shift lenght, default for shift length =1.0 if not known 
		//default configuration (N,N+) or (N,Nq)= (5,5)
		assign_type();
		m_mutation_strategy =  MutationDE::best_2;
		//update_archive(*m_inds[0]);
	}

	void SubPopDynDE::assign_type() {
		// for the version with the suggested settings 
		for (size_t i = 0; i < size(); ++i) {
			if (i < m_num_normal) m_inds[i]->m_type = IndDynDE::individual_type::TYPE_DE;
			else  m_inds[i]->m_type = IndDynDE::individual_type::TYPE_BROWNIAN;
		}
	}


	int SubPopDynDE::evolve(int id_pro, int id_alg, int id_rnd) {
		if (this->size() < 1)
			return kNormalEval;
		int tag = kNormalEval;
		for (size_t i = 0; i < this->size(); ++i) {
			if (m_inds[i]->m_type == IndDynDE::individual_type::TYPE_DE || m_inds[i]->m_type == IndDynDE::individual_type::TYPE_ENTROPY_DE) {
				mutate(i, id_rnd, id_pro);
				this->m_inds[i]->recombine(m_CR, m_recombine_strategy, id_rnd, id_pro);
			}
			else if (m_inds[i]->m_type == IndDynDE::individual_type::TYPE_BROWNIAN) {
				tag = m_inds[i]->brownian(*m_best.front(), m_sigma, id_pro, id_alg, id_rnd);

			}
			else if (m_inds[i]->m_type == IndDynDE::individual_type::TYPE_QUANTUM) {
				tag = m_inds[i]->quantum(*m_best.front(), m_r_cloud, id_pro, id_alg, id_rnd);
			}
			if (tag != kNormalEval)  return tag;
		}
		//this->updateIDnIndex();
		for (int i = 0; i < this->size(); i++) {
			if (m_inds[i]->m_type == IndDynDE::individual_type::TYPE_DE || m_inds[i]->m_type == IndDynDE::individual_type::TYPE_ENTROPY_DE) {
				tag = m_inds[i]->select(id_pro, id_alg);
				if (tag != kNormalEval) return tag;
			}

			if (m_inds[i]->m_type == IndDynDE::individual_type::TYPE_ENTROPY_DE) {
				// add entropy if necessary 
				tag = m_inds[i]->entropy(m_sigma, id_pro, id_alg, id_rnd);
				if (tag != kNormalEval) return tag;
			}
			//update_archive(*m_inds[i]);
		}
		if (tag == kNormalEval) {
			this->m_iter++;
		}
		return tag;
	}
}
