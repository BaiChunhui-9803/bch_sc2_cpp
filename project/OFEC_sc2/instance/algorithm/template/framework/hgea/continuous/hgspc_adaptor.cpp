#include "hgspc_adaptor.h"
#include "../../../../../../core/problem/continuous/continuous.h"
#include "../../../../../../utility/heap/heap.h"
#include "../../../../../../utility/functional.h"

namespace ofec {
	AdaptorHGSPC::AdaptorHGSPC(int id_pro, size_t init_num):
		m_opt_mode(GET_PRO(id_pro).optMode()),
		m_sp_tree(GET_PRO(id_pro).numVariables(), 2),
		m_info_ssp(init_num)
	{
		m_sp_tree.setInitBox(GET_CONOP(id_pro).boundary());
		m_sp_tree.inputRatioData(std::vector<Real>(init_num, 1. / init_num));
		m_sp_tree.buildIndex();
		for (size_t i = 0; i < init_num; ++i)
			m_sp_tree.findNeighbor(i, m_info_ssp[i].adjacent_ssp);
	}

	void AdaptorHGSPC::archiveSolution(const SolBase &sol) {
		AdaptorHGEA::archiveSolution(sol);
		auto p_sol = dynamic_cast<const Solution<>*>(m_his_sols.back().get());
		auto id_ssp = m_sp_tree.getRegionIdx(p_sol->variable().vect());
		m_info_ssp[id_ssp].his_sol.emplace_back(p_sol);
		if (!m_info_ssp[id_ssp].best_sol || p_sol->dominate(*m_info_ssp[id_ssp].best_sol, m_opt_mode))
			m_info_ssp[id_ssp].best_sol = p_sol;
	}

	void AdaptorHGSPC::updateValleyAndPeaks() {
		interpolateMissing();
		monotonicClustering();
	}

	int AdaptorHGSPC::inValleyOrPeaks_(const VarVec<Real> &var) {
		auto id_ssp = m_sp_tree.getRegionIdx(var.vect());
		return m_info_ssp[id_ssp].valley_or_peaks;
	}
	
	void AdaptorHGSPC::updatePeaksPotentials(int id_pro) {
		std::vector<Real> dev_var(m_num_peaks, 0), dev_obj(m_num_peaks, 0);
		for (size_t i = 0; i < m_num_peaks; i++) {
			std::vector<const Solution<> *> latest_sols;
			for (auto it = m_his_sols_peaks[i].rbegin(); it != m_his_sols_peaks[i].rend(); ++it) {
				if (latest_sols.size() > 10)
					break;
				latest_sols.push_back(dynamic_cast<const Solution<>*>(*it));
			}
			std::vector<std::vector<Real>> vars(GET_PRO(id_pro).numVariables(), std::vector<Real>(latest_sols.size()));
			std::vector<std::vector<Real>> objs(GET_PRO(id_pro).numObjectives(), std::vector<Real>(latest_sols.size()));

			for (size_t j = 0; j < GET_PRO(id_pro).numObjectives(); j++) {
				auto &range = m_obj_range[j];
				for (size_t k = 0; k < latest_sols.size(); k++)
					objs[j][k] = mapReal(latest_sols[k]->objective(j), range.first, range.second, 0.0, 1.0);
			}
			for (size_t j = 0; j < GET_PRO(id_pro).numVariables(); j++) {
				auto &range = GET_CONOP(id_pro).range(j);
				for (size_t k = 0; k < latest_sols.size(); k++)
					vars[j][k] = mapReal(latest_sols[k]->variable()[j], range.first, range.second, 0.0, 1.0);
			}

			Real mean, dev;
			for (size_t j = 0; j < GET_PRO(id_pro).numObjectives(); j++) {
				calMeanAndStd(objs[j], mean, dev);
				dev_obj[i] += dev;
			}
			dev_obj[i] /= GET_PRO(id_pro).numObjectives();
			for (size_t j = 0; j < GET_PRO(id_pro).numVariables(); j++) {
				calMeanAndStd(vars[j], mean, dev);
				dev_var[i] += dev;
			}
			dev_var[i] /= GET_PRO(id_pro).numVariables();
		}
		Real dev_var_min, dev_var_max;
		Real dev_obj_min, dev_obj_max;
		dev_var_min = dev_var_max = dev_var[0];
		dev_obj_min = dev_obj_max = dev_obj[0];
		for (size_t i = 1; i < m_num_peaks; i++) {
			if (dev_var_min > dev_var[i])
				dev_var_min = dev_var[i];
			if (dev_var_max < dev_var[i])
				dev_var_max = dev_var[i];
			if (dev_obj_min > dev_obj[i])
				dev_obj_min = dev_obj[i];
			if (dev_obj_max < dev_obj[i])
				dev_obj_max = dev_obj[i];
		}
		for (size_t i = 1; i < m_num_peaks; i++) {
			dev_var[i] = mapReal(dev_var[i], dev_var_min, dev_var_max, 0.0, 1.0);
			dev_obj[i] = mapReal(dev_obj[i], dev_obj_min, dev_obj_max, 0.0, 1.0);
		}

		m_potential_peaks.resize(m_num_peaks);
		for (size_t i = 1; i < m_num_peaks; i++)
			m_potential_peaks[i] = 0.5 * dev_var[i] + 0.5 * dev_obj[i];
	}

	void AdaptorHGSPC::updateAffiliationHisSols() {
		m_valley_his_sols.clear();
		m_his_sols_peaks.clear();
		m_his_sols_peaks.resize(m_num_peaks);
		for (auto &is : m_info_ssp) {
			if (is.valley_or_peaks == -1)
				m_valley_his_sols.insert(m_valley_his_sols.end(), is.his_sol.begin(), is.his_sol.end());
			else
				m_his_sols_peaks[is.valley_or_peaks].insert(m_his_sols_peaks[is.valley_or_peaks].end(), is.his_sol.begin(), is.his_sol.end());
		}
	}

	std::vector<Real> AdaptorHGSPC::uniquenessSols(std::vector<const SolBase*> &sols) const {
		std::vector<Real> result(sols.size());
		for (size_t i = 0; i < sols.size(); i++) {
			auto id_ssp = m_sp_tree.getRegionIdx(dynamic_cast<const Solution<>*>(sols[i])->variable().vect());
			result[i] = 1.0 / (m_info_ssp[id_ssp].his_sol.size() + 1);
		}
		return result;
	}

	std::vector<const SolBase*> AdaptorHGSPC::adjacentHisSols(const SolBase &sol2, size_t num, int id_rnd, int id_pro) const {
		std::vector<const SolBase*> result(num);
		std::vector<const SolBase*> vhs;
		for (auto sol : m_valley_his_sols)
			vhs.push_back(sol);
		GET_RND(id_rnd).uniform.shuffle(vhs.begin(), vhs.end());
		std::map<Real, const SolBase*> dis_sol;
		auto it1 = vhs.begin();
		for (size_t i = 0; i < 2 * num; it1++, i++)
			dis_sol[sol2.variableDistance(**it1, id_pro)] = *it1;
		auto it2 = dis_sol.begin();
		for (size_t i = 0; i < num; it2++, i++)
			result[i] = it2->second;
		return result;
	}
	
	void AdaptorHGSPC::interpolateMissing() {
		std::vector<size_t> unsampled_ssps;
		std::unordered_map<size_t, size_t> id_in_uspl_ssps;
		m_obj_ssp.resize(m_info_ssp.size());
		for (size_t i = 0; i < m_info_ssp.size(); i++) {
			if (!m_info_ssp[i].his_sol.empty())
				m_obj_ssp[i] = m_info_ssp[i].best_sol->objective(0);
			else {
				id_in_uspl_ssps[i] = unsampled_ssps.size();
				unsampled_ssps.push_back(i);
			}
		}
		std::vector<int> num_miss_nbrs(unsampled_ssps.size(), 0);
		for (size_t i = 0; i < m_info_ssp.size(); i++) {
			if (m_info_ssp[i].his_sol.empty()) {
				for (size_t n : m_info_ssp[i].adjacent_ssp) {
					if (id_in_uspl_ssps.count(n) > 0)
						num_miss_nbrs[id_in_uspl_ssps[n]]++;
				}
			}
		}
		Heap heap(num_miss_nbrs);
		std::set<size_t> interpolated;
		while (interpolated.size() < unsampled_ssps.size()) {
			size_t id_top = heap.top();
			size_t id_ssp = unsampled_ssps[id_top];
			m_obj_ssp[id_ssp] = 0;
			int num = 0;
			for (size_t n : m_info_ssp[id_ssp].adjacent_ssp) {
				if (!m_info_ssp[n].his_sol.empty() || interpolated.count(n) > 0) {
					m_obj_ssp[id_ssp] += m_obj_ssp[n];
					num++;
				}
			}
			m_obj_ssp[id_ssp] /= num;
			interpolated.insert(id_ssp);
			heap.remove(id_top);
			for (size_t n : m_info_ssp[id_ssp].adjacent_ssp) {
				if (id_in_uspl_ssps.count(n) > 0) {
					size_t id_in_heap = id_in_uspl_ssps.at(n);
					num_miss_nbrs[id_in_heap]--;
					heap.updateValue(id_in_heap, num_miss_nbrs[id_in_heap]);
				}
			}
		}
	}
	
	void AdaptorHGSPC::monotonicClustering() {
		std::multimap<Real, size_t> sort_fitness;
		std::vector<size_t> num_clustered(m_info_ssp.size(), 0);
		std::list<std::set<size_t>> clusters;
		for (size_t i = 0; i < m_info_ssp.size(); ++i)
			sort_fitness.emplace(m_obj_ssp[i], i);
		if (m_opt_mode[0] == OptMode::kMinimize) {
			for (auto i = sort_fitness.begin(); i != sort_fitness.end(); ++i) {
				if (num_clustered[i->second] == 0) {
					std::set<size_t> cluster;
					cluster.emplace(i->second);
					num_clustered[i->second]++;
					monotonicSearching(i->second, cluster, num_clustered);
					clusters.emplace_back(cluster);
				}
			}
		}
		else {
			for (auto i = sort_fitness.rbegin(); i != sort_fitness.rend(); ++i) {
				if (num_clustered[i->second] == 0) {
					std::set<size_t> cluster;
					cluster.emplace(i->second);
					num_clustered[i->second]++;
					monotonicSearching(i->second, cluster, num_clustered);
					clusters.emplace_back(cluster);
				}
			}
		}
		if (clusters.size() > 1) {
			m_ssp_peaks.clear();
			for (size_t i = 0; i < m_info_ssp.size(); ++i)
				m_info_ssp[i].valley_or_peaks = -2;
			m_num_peaks = 0;
			for (auto &cluster : clusters) {
				std::vector<size_t> c;
				for (auto i : cluster) {
					if (num_clustered[i] == 1) {
						m_info_ssp[i].valley_or_peaks = m_num_peaks;
						c.push_back(i);
					}
				}
				if (!c.empty()) {
					m_ssp_peaks.push_back(std::move(c));
					m_num_peaks++;
				}
			}
			m_ssp_valley.clear();
			for (size_t i = 0; i < num_clustered.size(); ++i) {
				if (num_clustered[i] > 1) {
					m_info_ssp[i].valley_or_peaks = -1;
					m_ssp_valley.push_back(i);
				}
			}
		}
		else {
			// TODO
			;
		}
	}
	
	void AdaptorHGSPC::monotonicSearching(size_t center, std::set<size_t> &cluster, std::vector<size_t> &num_clutered) {
		for (auto i : m_info_ssp[center].adjacent_ssp) {
			if ((m_opt_mode[0] == OptMode::kMinimize&& m_obj_ssp[i] >= m_obj_ssp[center])
				|| (m_opt_mode[0] == OptMode::kMaximize && m_obj_ssp[i] <= m_obj_ssp[center])) {
				if (cluster.count(i) == 0) {
					cluster.emplace(i);
					num_clutered[i]++;
					monotonicSearching(i, cluster, num_clutered);
				}
			}
		}
	}
}