#ifndef OFEC_HGEA_H
#define OFEC_HGEA_H

#include "../../../../../core/algorithm/algorithm.h"
#include "hgea_adaptor.h"

namespace ofec {
	class HGEA : public Algorithm {
	protected:
		size_t m_pop_size;
		Real m_alpha;         // ratio of exploitation to total
		std::unique_ptr<BaseAdaptorHGEA> m_adaptor;
		std::vector<std::unique_ptr<SolBase>> m_population, m_offspring;
		std::vector<std::vector<const SolBase*>> m_parents;

		std::vector<size_t> m_aff_inds_valley; // affiliated individuals of the valley
		std::vector<std::vector<size_t>> m_aff_inds_each_peak; // affiliated individuals of each peak
		std::vector<size_t> m_ind_in_valley_or_peaks; // individual is in the valley or peaks (-1: valley, >0: the index of peaks)
		std::vector<size_t> m_off_in_valley_or_peaks; // offspring is in the valley or peaks (-1: valley, >0: the index of peaks)
	
		void initialize_() override;
		void run_() override;
		void record() override {}
#ifdef OFEC_DEMO
		virtual void updateBuffer() = 0;
#endif

		virtual void initializeSolution(size_t id_ind) = 0;
		virtual void reproduceSolution(size_t id_ind) = 0;

	private:
		void initialization();
		void updateAffiliations();
		void parentsSelection();
		void reproduction();
		void evaluation();
		void survivorSelection();
	};
}

#endif // !OFEC_HGEA_H
