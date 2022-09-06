/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* APC (affinity propagation clustering)
* Frey, Brendan J., and Delbert Dueck. 
* "Clustering by Passing Messages Between Data Points." 
* Science 315.5814 (2007): 972-976.
*-------------------------------------------------------------------------------
* Constructed by Junchen Wang (wangjunchen.chris@gmail.com) at 2020/5/13
*********************************************************************************/

#ifndef OFEC_APC_H
#define OEFC_APC_H

#include <algorithm>
#include <vector>
#include <set>
#include <numeric>

#include"../../core/algorithm/population.h"
#include"../../core/algorithm/individual.h"

#include<fstream>

namespace ofec {
	template<typename TInd>
	class APC {
		const Real m_lambda;
		const size_t m_Mits, m_Cits;
		size_t m_N; // Size of data
		std::vector<std::vector<Real>> m_responsibility;
		std::vector<std::vector<Real>> m_availability;
		std::vector<std::vector<Real>> m_similarity;
		std::vector<const typename TInd::SolType*> m_data;
		std::vector<std::vector<size_t>> m_clusters;
	public:
		APC(Real lambda, size_t Mits, size_t Cits) : m_lambda(lambda), m_Mits(Mits), m_Cits(Cits) {}
		void updateData(const Population<TInd>& pop);
		void updateData(const std::vector<TInd>& inds);
		void clustering(int id_pro);
		const std::vector<std::vector<size_t>>& clusters() const { return m_clusters; }
	protected:
		void updateSimilarity(int id_pro);
		void updateResponsibility();
		void updateAvailability();
		void updateClusters();
	};
	template<typename TInd>
	void APC<TInd>::updateData(const Population<TInd>& pop){
		m_N = pop.size();
		if (m_responsibility.size() != m_N) {
			m_responsibility.resize(m_N);
		}
		for (auto& row : m_responsibility)
			row.assign(m_N, 0);
		if (m_availability.size() != m_N) {
			m_availability.resize(m_N);
		}
		for (auto& row : m_availability)
			row.assign(m_N, 0);
		if (m_similarity.size() != m_N) {
			m_similarity.resize(m_N);
			for (auto& row : m_similarity)
				row.resize(m_N);
		}
		if (!m_data.size() != m_N)
			m_data.resize(m_N);
		for (size_t i = 0; i < m_N; ++i)
			m_data[i] = &pop[i].solut();
		m_clusters.clear();
	}

	template<typename TInd>
	void APC<TInd>::updateData(const std::vector<TInd>& inds) {
		m_N = inds.size();
		if (m_responsibility.size() != m_N) {
			m_responsibility.resize(m_N);
			for (auto& row : m_responsibility)
				row.assign(m_N, 0);
		}
		if (m_availability.size() != m_N) {
			m_availability.resize(m_N);
			for (auto& row : m_availability)
				row.assign(m_N, 0);
		}
		if (m_similarity.size() != m_N) {
			m_similarity.resize(m_N);
			for (auto& row : m_similarity)
				row.resize(m_N);
		}
		if (!m_data.size() != m_N)
			m_data.resize(m_N);
		for (size_t i = 0; i < m_N; ++i)
			m_data[i] = &inds[i].solut();
		m_clusters.clear();
	}

	template<typename TInd>
	void APC<TInd>::clustering(int id_pro) {
		updateSimilarity(id_pro);
		for (size_t iter = 0; iter < m_Mits; iter++) {
			updateResponsibility();
			updateAvailability();
		}
		updateClusters();
	}

	template<typename TInd>
	void APC<TInd>::updateSimilarity(int id_pro) {
		std::vector<Real> all_similarities;
		for (size_t i = 0; i < m_N; i++) {
			for (size_t j = i + 1; j < m_N; j++) {
				m_similarity[i][j] = m_similarity[j][i] = -pow(m_data[i]->variableDistance(*m_data[j], id_pro), 2);
				all_similarities.emplace_back(m_similarity[i][j]);
			}
		}
		std::sort(all_similarities.begin(), all_similarities.end());
		Real median;
		size_t size = all_similarities.size();
		if (size % 2 == 0)
			median = (all_similarities[size / 2] + all_similarities[size / 2 - 1]) / 2;
		else
			median = all_similarities[size / 2];
		for (size_t i = 0; i < m_N; i++) {
			m_similarity[i][i] = median;
		}
	}

	template<typename TInd>
	void APC<TInd>::updateResponsibility() {
		std::vector<Real> max(m_N), sec(m_N);
		for (size_t i = 0; i < m_N; i++) {
			max[i] = std::max(m_availability[i][0] + m_similarity[i][0], m_availability[i][1] + m_similarity[i][1]);
			sec[i] = std::min(m_availability[i][0] + m_similarity[i][0], m_availability[i][1] + m_similarity[i][1]);
			for (size_t k = 3; k < m_N; k++) {
				Real val = m_availability[i][k] + m_similarity[i][k];
				if (val < sec[i]) 
					continue;
				else if (val < max[i]) 
					sec[i] = val;
				else {
					sec[i] = max[i]; max[i] = val;
				}
			}
		}
		for (size_t i = 0; i < m_N; i++) {
			for (size_t k = 0; k < m_N; k++) {
				if (m_availability[i][k] + m_similarity[i][k] == max[i])
					m_responsibility[i][k] = m_lambda * m_responsibility[i][k] + (1 - m_lambda) * (m_similarity[i][k] - sec[i]);
				else
					m_responsibility[i][k] = m_lambda * m_responsibility[i][k] + (1 - m_lambda) * (m_similarity[i][k] - max[i]);
			}
		}
	}

	template<typename TInd>
	void APC<TInd>::updateAvailability() {
		std::vector<Real> sum(m_N, 0);
		for (size_t i = 0; i < m_N; i++) {
			for (size_t k = 0; k < m_N; k++) {
				sum[i] += std::max<Real>(0, m_responsibility[i][k]);
			}
		}
		for (size_t i = 0; i < m_N; i++) {
			for (size_t k = 0; k < m_N; k++) {
				if (i == k)
					m_availability[i][k] = m_lambda * m_availability[i][k] + (1 - m_lambda) * (sum[i] - std::max<Real>(0, m_responsibility[i][k]));
				else
					m_availability[i][k] = m_lambda * m_availability[i][k] + (1 - m_lambda) * (std::min<Real>(0, m_responsibility[k][k] + (sum[i] - std::max<Real>(0, m_responsibility[i][k]) - std::max<Real>(0, m_responsibility[k][k]))));
			}
		}
	}

	template<typename TInd>
	void APC<TInd>::updateClusters() {
		std::set<size_t> set_exemplars;
		std::vector<size_t> vec_exemplars;
		for (size_t i = 0; i < m_N; i++) {
			if (m_responsibility[i][i] + m_availability[i][i] > 0) {
				set_exemplars.emplace(i);
				vec_exemplars.emplace_back(i);
				m_clusters.emplace_back(std::vector<size_t>({ i }));
			}
		}
		if (m_clusters.empty()) {
			std::vector<size_t> all_inds(m_data.size());
			std::iota(all_inds.begin(), all_inds.end(), 0);
			m_clusters.emplace_back(all_inds);
			return;
//			std::ofstream out_file("C:/Users/WJC/Desktop/data.csv");
//			for (size_t i = 0; i < m_N; i++) {
//				for (Real val : m_data[i]->variable().vect())
//					out_file << val << ",";
//				out_file << std::endl;
//			}
			//throw("error at updateClusters");
		}
		for (size_t i = 0; i < m_N; i++) {
			if (set_exemplars.count(i) == 1) continue;
			size_t idx = 0;
			for (size_t k = 1; k < vec_exemplars.size(); ++k) {
				if (m_similarity[i][vec_exemplars[k]] > m_similarity[i][vec_exemplars[idx]])
					idx = k;
			}
			m_clusters[idx].emplace_back(i);
		}
	}
}

#endif // !OFEC_APC_H

