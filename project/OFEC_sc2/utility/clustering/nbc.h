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
* NBC (Nearest-Better Clustering)
* Preuss, Mike. 
* "Niching the CMA-ES via nearest-better clustering." 
* Proceedings of the 12th annual conference companion on Genetic and evolutionary computation. 2010.
*-------------------------------------------------------------------------------
* Implemented by Junchen Wang (wangjunchen.chris@gmail.com) at 2020/9/22
*********************************************************************************/

#ifndef OFEC_NBC_H
#define OFEC_NBC_H

#include <algorithm>
#include <vector>
#include <deque>
#include <list>

#include "../../core/algorithm/population.h"
#include "../../core/algorithm/individual.h"

namespace ofec {
	template<typename TInd>
	class NBC {
	protected:
		Real m_phi;
		size_t m_N;                 // size of data
		std::vector<const typename TInd::SolType*> m_data;
		std::vector<std::vector<size_t>> m_clusters;
		std::vector<std::vector<Real>> m_distance;
		std::vector<int> m_graph;   // target node, if -1 means no target node
	public:
		NBC(Real phi = 2) : m_phi(phi) {}
		void updateData(const Population<TInd>& pop);
		void updateData(const std::vector<TInd>& inds);
		void clustering(int id_pro);
		const std::vector<std::vector<size_t>>& clusters() const { return m_clusters; }
	protected:
		void updateDistance(int id_pro);
		void updateGraph(int id_pro);
		void updateClusters();
	};

	template<typename TInd>
	void NBC<TInd>::updateData(const Population<TInd>& pop) {
		m_N = pop.size();
		if (m_distance.size() != m_N) {
			m_distance.resize(m_N);
			for (auto& row : m_distance)
				row.resize(m_N);
		}
		m_graph.resize(m_N);
		if (!m_data.size() != m_N)
			m_data.resize(m_N);
		for (size_t i = 0; i < m_N; ++i)
			m_data[i] = &pop[i].solut();
		if (!m_clusters.empty())
			m_clusters.clear();
	}

	template<typename TInd>
	void NBC<TInd>::updateData(const std::vector<TInd>& inds) {
		m_N = inds.size();
		if (m_distance.size() != m_N) {
			m_distance.resize(m_N);
			for (auto& row : m_distance)
				row.resize(m_N);
		}
		m_graph.resize(m_N);
		if (!m_data.size() != m_N)
			m_data.resize(m_N);
		for (size_t i = 0; i < m_N; ++i)
			m_data[i] = &inds[i].solut();
		if (!m_clusters.empty())
			m_clusters.clear();
	}

	template<typename TInd>
	void NBC<TInd>::clustering(int id_pro) {
		updateDistance(id_pro);
		updateGraph(id_pro);
		updateClusters();
	}

	template<typename TInd>
	void NBC<TInd>::updateDistance(int id_pro) {
		for (size_t i = 0; i < m_N; i++) {
			for (size_t j = i + 1; j < m_N; j++) {
				m_distance[i][j] = m_distance[j][i] = m_data[i]->variableDistance(*m_data[j], id_pro);
			}
		}
	}


	template<typename TInd>
	void NBC<TInd>::updateGraph(int id_pro) {
		Real mean_lenth(0);
		size_t num_edges(0);                 // reset the graph
		m_graph.assign(m_N, -1);
		for (size_t i = 0; i < m_N; i++) {
			std::map<Real, size_t> seq_dis;           // the default hehavior is in ascending order
			for (size_t j = 0; j < m_N; j++) {
				if (j != i)
					seq_dis.emplace(std::make_pair(m_distance[i][j], j));
			}
			for (const auto& p : seq_dis) {
				if (m_data[p.second]->dominate(*m_data[i], id_pro)) {
					m_graph[i] = p.second;
					num_edges++;
					mean_lenth += m_distance[i][p.second];
					break;
				}
			}
		}
		mean_lenth /= num_edges;
		for (size_t i = 0; i < m_N; i++) {            // cut edges too long
			if (m_graph[i] != -1 && m_distance[i][m_graph[i]] > m_phi * mean_lenth)
				m_graph[i] = -1;
		}
	}

	template<typename TInd>
	void NBC<TInd>::updateClusters() {
		std::vector<int> idxs_clu(m_N, -1);
		size_t  idx_clu(0), idx_cur_ind;
		for (size_t i = 0; i < m_N; i++) {
			if (idxs_clu[i] == -1) {
				std::list<size_t> path;
				idx_cur_ind = i;
				path.emplace_back(idx_cur_ind);
				while (m_graph[idx_cur_ind] != -1) {
					idx_cur_ind = m_graph[idx_cur_ind];
					path.emplace_back(idx_cur_ind);
				}
				if (idxs_clu[idx_cur_ind] == -1)
					idxs_clu[idx_cur_ind] = idx_clu++;
				for (size_t idx_ind : path) {
					idxs_clu[idx_ind] = idxs_clu[idx_cur_ind];
					m_graph[idx_ind] = -1;
				}
			}
		}
		m_clusters.clear();
		m_clusters.resize(idx_clu);
		for (size_t i = 0; i < m_N; ++i)
			m_clusters[idxs_clu[i]].emplace_back(i);
	}
}

#endif // !OFEC_NBC_H
