#include "./hlc.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../utility/nondominated_sorting/fast_sort.h"
#include "../../../../../utility/functional.h"
#include "../../../../../utility/nondominated_sorting/filter_sort.h"
#include <numeric>
#include "logarithmic.h"
#include "../../../global/canonical_de/canonical_de.h"
#include "../../../../../utility/heap/heap.h"
#include <unordered_set>
#include <unordered_map>
#include "../../../../../utility/clustering/fdc.h"
#include "../../../../../utility/clustering/hslh.h"
#include "../../../../../utility/clustering/dpc.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	HLC::HLC(size_t size_var, size_t num_divs) :
		m_num_divs(num_divs),
		m_size_var(size_var),
		m_opt_mode_sol(1),
		m_opt_mode_bsn_atrct(2) {}

	void HLC::initialize(
		const std::vector<std::pair<Real, Real>> &boundary, OptMode opt_mode, int init_num)
	{
		m_opt_mode_sol[0] = opt_mode;
		m_opt_mode_bsn_atrct[0] = opt_mode;
		m_opt_mode_bsn_atrct[1] = OptMode::kMinimize;
		m_subspace_tree.reset(new SplitTree(m_size_var, 2));
		m_subspace_tree->setInitBox(boundary);
		m_subspace_tree->inputRatioData(std::vector<Real>(init_num, 1. / init_num));
		m_subspace_tree->buildIndex();
		m_info_subspaces.clear();
		for (size_t i = 0; i < init_num; ++i) {
			m_info_subspaces.emplace_back(new InfoSubspace);
			m_info_subspaces[i]->num_imprv = 0;
			//m_info_subspaces[i]->diff = 1;
			m_subspace_tree->findNeighbor(i, m_info_subspaces[i]->neighbors);
		}			

		m_info_bsns_atrct.clear();
		m_info_bsns_atrct.emplace_back(new InfoBsnAtrct);

		m_info_bsns_atrct[0]->potential = 1;

		m_info_bsns_atrct[0]->subspaces.resize(init_num);
		std::iota(m_info_bsns_atrct[0]->subspaces.begin(), m_info_bsns_atrct[0]->subspaces.end(), 0);
		for (size_t i = 0; i < init_num; i++)
			m_info_subspaces[i]->aff_bsn_atr = 0;

		m_info_bsns_atrct[0]->vols_ssps.resize(init_num);
		for (size_t i = 0; i < init_num; i++)
			m_info_bsns_atrct[0]->vols_ssps[i] = m_subspace_tree->getBoxVolume(i);

		m_info_bsns_atrct[0]->volume = 0;
		for (size_t i = 0; i < init_num; i++)
			m_info_bsns_atrct[0]->volume += m_subspace_tree->getBoxVolume(i);

		m_info_bsns_atrct[0]->prob_ssps.resize(init_num);
		for (size_t i = 0; i < init_num; i++)
			m_info_bsns_atrct[0]->prob_ssps[i] = 1;

		for (size_t i = 0; i < init_num; i++)
			m_info_bsns_atrct[0]->seq_ssps[i] = i;

		if (m_num_divs > 0)
			divideBsnAtrct(0);
	}

	void HLC::inputSample(const Solution<> &sol, bool improved) {
		m_his_inds.push_back(std::make_shared<const Solution<>>(sol));
		if (!m_best_sol || m_his_inds.back()->dominate(*m_best_sol, m_opt_mode_sol))
			m_best_sol = m_his_inds.back();
		if (!m_worst_sol || m_worst_sol->dominate(*m_his_inds.back(), m_opt_mode_sol))
			m_worst_sol = m_his_inds.back();
		size_t id_ssp = m_subspace_tree->getRegionIdx(m_his_inds.back()->variable().vect());
		updateSubspaceInfo(id_ssp, m_his_inds.back());
		size_t id_bsn = m_info_subspaces[id_ssp]->aff_bsn_atr;
		updateBsnAtrctInfo(id_bsn, m_his_inds.back());
		if (m_num_divs > 0)
			m_info_bsns_atrct[id_bsn]->freq_divs[getIdxDiv(id_bsn, sol)]++;
		//if (improved)
		//	m_info_subspaces[id_ssp]->improved_sols.push_back(m_his_inds.back());
	}

	void HLC::inputSurvivor(const Solution<> &sample_point) {
		m_info_subspaces[getIdxSSP(sample_point)]->survivors.push_back(std::make_shared<const Solution<>>(sample_point));
	}

	//void HLC::updateImprovedSample(const std::vector<const Solution<>*> &pop) {
	//	for (auto sol : pop) {
	//		size_t id_ssp = m_subspace_tree->getRegionIdx(sol->variable().vect());
	//		m_info_subspaces[id_ssp]->improved_sols.push_back(std::make_shared<const Solution<>>(*sol));
	//	}
	//}

	void HLC::divideBsnAtrct(size_t id_bsn_atrct) {
		auto &info = *m_info_bsns_atrct.at(id_bsn_atrct);
		info.vols_divs.clear();
		info.div_to_ssp_atp.clear();
		info.ssp_atp_to_div.clear();
		info.cov_divs.clear();
		info.cov_divs.resize(m_num_divs);
		for (size_t id_subspace : info.subspaces)
			m_info_subspaces.at(id_subspace)->atomspace_tree.reset();
		if (info.subspaces.size() <= m_num_divs) {
			for (size_t id_subspace : info.subspaces) {
				info.vols_divs.emplace_back(m_subspace_tree->getBoxVolume(id_subspace));
				info.ssp_atp_to_div[id_subspace][-1] = info.div_to_ssp_atp.size();
				info.div_to_ssp_atp.push_back({ std::make_pair(id_subspace, -1) });
			}
			while (info.div_to_ssp_atp.size() < m_num_divs) {
				size_t div_max = 0;
				for (size_t i = 1; i < info.vols_divs.size(); i++) {
					if (info.vols_divs[i] > info.vols_divs[div_max])
						div_max = i;
				}
				size_t aff_ssp = info.div_to_ssp_atp[div_max].front().first;
				if (m_info_subspaces.at(aff_ssp)->atomspace_tree) {
					size_t aff_asp = info.div_to_ssp_atp[div_max].front().second;
					size_t new_asp = m_info_subspaces.at(aff_ssp)->atomspace_tree->splitRegion(aff_asp);
					info.ssp_atp_to_div[aff_ssp][new_asp] = info.div_to_ssp_atp.size();
					info.div_to_ssp_atp.push_back({ std::make_pair(aff_ssp, new_asp) });
				}
				else {
					m_info_subspaces.at(aff_ssp)->atomspace_tree.reset(new SplitTree(m_size_var, 2));
					m_info_subspaces.at(aff_ssp)->atomspace_tree->setInitBox(m_subspace_tree->getBox(aff_ssp));
					m_info_subspaces.at(aff_ssp)->atomspace_tree->inputRatioData({ 0.5,0.5 });
					m_info_subspaces.at(aff_ssp)->atomspace_tree->buildIndex();
					info.ssp_atp_to_div[aff_ssp][0] = div_max;
					info.div_to_ssp_atp[div_max].front().second = 0;
					info.ssp_atp_to_div[aff_ssp][1] = info.div_to_ssp_atp.size();
					info.div_to_ssp_atp.push_back({ std::make_pair(aff_ssp, 1) });
				}
				info.vols_divs[div_max] /= 2;
				info.vols_divs.push_back(info.vols_divs[div_max]);
			}
		}
		else {
			std::unordered_set<size_t> ssps;
			std::unordered_map<size_t, size_t> ids_in_bsn;
			std::vector<size_t> aff_ssp(info.subspaces.size());
			std::vector<Real> volume(info.subspaces.size());
			std::vector<Real> min_sum_vol(info.subspaces.size());
			std::vector<std::vector<size_t>> nbrs(info.subspaces.size());
			std::vector<std::unordered_map<size_t, int>> ids_in_nbr(info.subspaces.size());
			std::vector<Heap<Real>> nbrs_heap;
			std::vector<std::list<size_t>> cncld_ssps(info.subspaces.size());
			size_t id_in_bsn = 0;
			for (size_t id_subspace : info.subspaces) {
				ssps.insert(id_subspace);
				volume[id_in_bsn++] = m_subspace_tree->getBoxVolume(id_subspace);
			}
			id_in_bsn = 0;
			for (size_t id_subspace : info.subspaces) {
				ids_in_bsn[id_subspace] = id_in_bsn;
				aff_ssp[id_in_bsn] = id_subspace;
				cncld_ssps[id_in_bsn].push_back(id_subspace);
				std::vector<Real> nbr_vols;
				size_t id_in_nbr = 0;
				for (size_t nbr_ssp : m_info_subspaces.at(id_subspace)->neighbors) {
					if (ssps.count(nbr_ssp)) {
						nbrs[id_in_bsn].push_back(nbr_ssp);
						ids_in_nbr[id_in_bsn][nbr_ssp] = id_in_nbr++;
						nbr_vols.push_back(m_subspace_tree->getBoxVolume(nbr_ssp));
					}
				}
				nbrs_heap.emplace_back(nbr_vols);
				size_t nbr_min = nbrs[id_in_bsn][nbrs_heap.back().top()];
				min_sum_vol[id_in_bsn] = volume[id_in_bsn] + volume[ids_in_bsn[nbr_min]];
				id_in_bsn++;
			}

			Heap heap(min_sum_vol);
			std::set<size_t> del_ssps;
			while (del_ssps.size() < info.subspaces.size() - m_num_divs) {
				id_in_bsn = heap.top();
				size_t id_ssp_to_merge = info.subspaces[id_in_bsn];
				size_t id_ssp_merged = nbrs[id_in_bsn][nbrs_heap[id_in_bsn].top()];
				volume[id_in_bsn] += volume[ids_in_bsn[id_ssp_merged]];
				for (size_t ssp : cncld_ssps[ids_in_bsn[id_ssp_merged]]) {
					aff_ssp[ids_in_bsn[ssp]] = aff_ssp[id_in_bsn];
					cncld_ssps[id_in_bsn].push_back(ssp);
				}
				cncld_ssps[ids_in_bsn[id_ssp_merged]].clear();
				for (size_t n : nbrs[ids_in_bsn[id_ssp_merged]]) {
					if (del_ssps.count(n) > 0 || n == id_ssp_to_merge)
						continue;
					if (ids_in_nbr[ids_in_bsn[n]].count(id_ssp_to_merge) == 0) {
						nbrs[ids_in_bsn[n]][ids_in_nbr[ids_in_bsn[n]][id_ssp_merged]] = id_ssp_to_merge;
						ids_in_nbr[ids_in_bsn[n]][id_ssp_to_merge] = ids_in_nbr[ids_in_bsn[n]][id_ssp_merged];
						ids_in_nbr[ids_in_bsn[n]][id_ssp_merged] = -1;
						nbrs_heap[ids_in_bsn[n]].updateValue(ids_in_nbr[ids_in_bsn[n]][id_ssp_to_merge], volume[id_in_bsn]);

						ids_in_nbr[id_in_bsn][n] = nbrs[id_in_bsn].size();
						nbrs[id_in_bsn].push_back(n);
						nbrs_heap[id_in_bsn].add(volume[ids_in_bsn[n]]);
					}
					else {
						nbrs_heap[ids_in_bsn[n]].remove(ids_in_nbr[ids_in_bsn[n]][id_ssp_merged]);
						nbrs_heap[ids_in_bsn[n]].updateValue(ids_in_nbr[ids_in_bsn[n]][id_ssp_to_merge], volume[id_in_bsn]);
					}
					size_t nbr_min = nbrs[ids_in_bsn[n]][nbrs_heap[ids_in_bsn[n]].top()];
					min_sum_vol[ids_in_bsn[n]] = volume[ids_in_bsn[n]] + volume[ids_in_bsn[nbr_min]];
					heap.updateValue(ids_in_bsn[n], min_sum_vol[ids_in_bsn[n]]);
				}
				for (size_t n : nbrs[id_in_bsn]) {
					if (del_ssps.count(n) > 0)
						continue;
					nbrs_heap[ids_in_bsn[n]].updateValue(ids_in_nbr[ids_in_bsn[n]][id_ssp_to_merge], volume[id_in_bsn]);
					size_t nbr_min = nbrs[ids_in_bsn[n]][nbrs_heap[ids_in_bsn[n]].top()];
					min_sum_vol[ids_in_bsn[n]] = volume[ids_in_bsn[n]] + volume[ids_in_bsn[nbr_min]];
					heap.updateValue(ids_in_bsn[n], min_sum_vol[ids_in_bsn[n]]);
				}

				nbrs_heap[id_in_bsn].remove(ids_in_nbr[id_in_bsn][id_ssp_merged]);
				size_t nbr_min = nbrs[id_in_bsn][nbrs_heap[id_in_bsn].top()];
				min_sum_vol[id_in_bsn] = volume[id_in_bsn] + volume[ids_in_bsn[nbr_min]];
				heap.updateValue(id_in_bsn, min_sum_vol[id_in_bsn]);

				heap.remove(ids_in_bsn[id_ssp_merged]);
				del_ssps.insert(id_ssp_merged);
			}

			std::unordered_map<size_t, size_t> ssps_to_div;
			size_t id_div = 0;
			for (size_t id_subspace : info.subspaces) {
				if (ssps_to_div.count(aff_ssp[ids_in_bsn[id_subspace]]) == 0) {
					ssps_to_div[aff_ssp[ids_in_bsn[id_subspace]]] = id_div++;
				}
			}
			info.div_to_ssp_atp.resize(id_div);
			for (size_t id_subspace : info.subspaces) {
				id_div = ssps_to_div[aff_ssp[ids_in_bsn[id_subspace]]];
				info.div_to_ssp_atp[id_div].emplace_back(id_subspace, -1);
				info.ssp_atp_to_div[id_subspace][-1] = id_div;
			}
			info.vols_divs.resize(m_num_divs);
			for (size_t i = 0; i < m_num_divs; i++) {
				info.vols_divs[i] = 0;
				for (auto &div : info.div_to_ssp_atp[i]) {
					info.vols_divs[i] += m_subspace_tree->getBoxVolume(div.first);
				}
			}
		}
		info.freq_divs.assign(m_num_divs, 0);
		for (size_t id_ssp : info.subspaces) {
			for (auto &sol : m_info_subspaces[id_ssp]->sols) {
				info.freq_divs[getIdxDiv(id_bsn_atrct, *sol)]++;
			}
		}
	}

	void HLC::updateSubspaceInfo(size_t id_ssp, const std::shared_ptr<const Solution<>> &ptr_sol) {
		auto &info = *m_info_subspaces[id_ssp];
		if (!info.best_sol || ptr_sol->dominate(*info.best_sol, m_opt_mode_sol)) {
			info.obj = ptr_sol->objective(0);
			info.best_sol = ptr_sol;
			info.num_imprv++;
			//info.good_sols.push_back(ptr_sol);
		}
		if (!info.worst_sol || info.worst_sol->dominate(*ptr_sol, m_opt_mode_sol)) {
			info.worst_sol = ptr_sol;
		}
		info.sols.push_back(ptr_sol);
		//info.diff = 1.0 - Real(info.num_imprv) / info.sols.size();
		//info.heap_objs.insert(ptr_sol->objective(0));
		//bool good;
		//if (m_opt_mode_sol[0] == OptMode::kMinimize)
		//	good = ptr_sol->objective(0) <= info.heap_objs.median();
		//else
		//	good = ptr_sol->objective(0) >= info.heap_objs.median();
		//if (good)
		//	info.good_sols.push_back(ptr_sol);
	}

	void HLC::updateBsnAtrctInfo(size_t id_bsn, const std::shared_ptr<const Solution<>> &ptr_sol) {
		auto &info = *m_info_bsns_atrct[id_bsn];
		if (!info.best_sol || ptr_sol->dominate(*info.best_sol, m_opt_mode_sol))
			info.best_sol = ptr_sol;
		if (!info.worst_sol || info.worst_sol->dominate(*ptr_sol, m_opt_mode_sol))
			info.worst_sol = ptr_sol;
	}

	bool HLC::isAdjacent(const std::vector<std::pair<Real, Real>> &box1, const std::vector<std::pair<Real, Real>> &box2) {
		bool one_point_adjacent = true;
		for (size_t j = 0; j < box1.size(); ++j) {
			if (box1[j].second < box2[j].first || box2[j].second < box1[j].first)
				return false;
			else if (box1[j].second > box2[j].first && box2[j].second > box1[j].first)
				one_point_adjacent = false;
		}
		return !one_point_adjacent;
	}

	void HLC::regressCovLines() {
		//std::vector<std::thread> thrds;
		//for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
		//	if (m_info_bsns_atrct.at(i).coverages.empty()) continue;
		//	thrds.push_back(std::thread(&HLC::regressCovLine, this, i));
		//}
		//for (auto &thrd : thrds)
		//	thrd.join();
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			regressCovLine(i);
		}
	}

	void HLC::interpolate() {
		// Interpolation of objective values in the unsampled subspaces
		std::vector<size_t> unsampled_ssps;
		std::unordered_map<size_t, size_t> id_in_uspl_ssps;
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (m_info_subspaces[i]->sols.empty()) {
				id_in_uspl_ssps[i] = unsampled_ssps.size();
				unsampled_ssps.push_back(i);
			}
		}
		std::vector<int> num_miss_nbrs(unsampled_ssps.size(), 0);
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (m_info_subspaces[i]->sols.empty()) {
				for (size_t n : m_info_subspaces.at(i)->neighbors) {
					if (id_in_uspl_ssps.count(n) > 0)
						num_miss_nbrs[id_in_uspl_ssps[n]]++;
				}
			}
		}
		Heap heap(num_miss_nbrs);
		m_interpolated.clear();
		while (m_interpolated.size() < unsampled_ssps.size()) {
			size_t id_top = heap.top();
			size_t id_ssp = unsampled_ssps[id_top];
			m_info_subspaces[id_ssp]->obj = 0;
			int num = 0;
			for (size_t n : m_info_subspaces.at(id_ssp)->neighbors) {
				if (!m_info_subspaces[n]->sols.empty() || m_interpolated.count(n) > 0) {
					m_info_subspaces[id_ssp]->obj += m_info_subspaces[n]->obj;
					num++;
				}
			}
			m_info_subspaces[id_ssp]->obj /= num;
			m_interpolated.insert(id_ssp);
			heap.remove(id_top);
			for (size_t n : m_info_subspaces.at(id_ssp)->neighbors) {
				if (id_in_uspl_ssps.count(n) > 0) {
					size_t id_in_heap = id_in_uspl_ssps.at(n);
					num_miss_nbrs[id_in_heap]--;
					heap.updateValue(id_in_heap, num_miss_nbrs[id_in_heap]);
				}
			}
		}
	}

	void HLC::regressCovLine(size_t id_basin) {
		std::vector<Real> iters(m_info_bsns_atrct.at(id_basin)->cuml_covs.size());
		std::iota(iters.begin(), iters.end(), 1);
		ParamMap param;

		//param["problem name"] = std::string("logarithmic regression for SPAE");
		//param["algorithm name"] = std::string("DE/rand/1");
		param["mutation strategy"] = static_cast<int>(MutationDE::rand_1);
		param["population size"] = static_cast<int>(20);
		param["maximum evaluations"] = static_cast<int>(2000);
		std::unique_ptr<Problem> pro(new RegressLog(iters, m_info_bsns_atrct.at(id_basin)->cuml_covs));
		std::unique_ptr<Algorithm> alg(new Canonical_DE);
		int id_param = ADD_PARAM(param);
		int id_pro = ADD_PRO_EXST(id_param, pro, 0.5);
		int id_alg = ADD_ALG_EXST(id_param, alg, id_pro, 0.5, -1);
		GET_PRO(id_pro).initialize();
		GET_ALG(id_alg).initialize();
		GET_ALG(id_alg).setKeepCandidatesUpdated(true);

		GET_ALG(id_alg).run();
		auto best = dynamic_cast<Solution<>*>(GET_ALG(id_alg).candidates().front().get());
		m_info_bsns_atrct.at(id_basin)->log_base = best->variable()[0];
		//m_info_bsns_atrct.at(id_basin).log_param[2] = best->variable()[2];
		DEL_ALG(id_alg);
		DEL_PRO(id_pro);
		DEL_PARAM(id_param);
	}

	std::map<size_t, size_t> HLC::clusterInBsnAtrct(size_t id_bsn) const {
		std::vector<bool> in_bsn(m_info_subspaces.size(), false);
		for (size_t id_ssp : m_info_bsns_atrct[id_bsn]->subspaces)
			in_bsn[id_ssp] = true;

		/* merge adjacent subspaces of equal fit */
		std::vector<size_t> belonged(m_info_subspaces.size());
		std::vector<std::set<size_t>> neighbors(m_info_subspaces.size());
		std::vector<bool> visited(m_info_subspaces.size(), false);
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (!in_bsn[i])	continue;
			if (!visited[i]) {
				belonged[i] = i;
				for (size_t nn : m_info_subspaces[i]->neighbors) {
					if (in_bsn[nn])
						neighbors[i].insert(nn);
				}
				std::list<int> neis;
				neis.push_back(i);
				visited[i] = true;
				while (!neis.empty()) {
					int cur_id = neis.front();
					neis.pop_front();
					for (size_t n : m_info_subspaces[cur_id]->neighbors) {
						if (in_bsn[n] && m_info_subspaces[cur_id]->obj == m_info_subspaces[n]->obj && !visited[n]) {
							belonged[n] = i;
							for (size_t nn : m_info_subspaces[n]->neighbors) {
								if (in_bsn[nn])
									neighbors[i].insert(nn);
							}
							neis.push_back(n);
							visited[n] = true;
						}
					}
				}
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++)
			if (in_bsn[i])
				neighbors[belonged[i]].erase(i);
		std::vector<size_t> best_neighbor(m_info_subspaces.size());
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (!in_bsn[i]) continue;
			if (belonged[i] != i) continue;
			best_neighbor[i] = i;
			for (size_t n : neighbors[i]) {
				if ((m_opt_mode_bsn_atrct[0] == OptMode::kMinimize && m_info_subspaces[best_neighbor[i]]->obj > m_info_subspaces[n]->obj)
					|| (m_opt_mode_bsn_atrct[0] == OptMode::kMaximize && m_info_subspaces[best_neighbor[i]]->obj < m_info_subspaces[n]->obj))
					best_neighbor[i] = belonged[n];
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (!in_bsn[i]) continue;
			if (belonged[i] == i) {
				while (best_neighbor[best_neighbor[i]] != best_neighbor[i]) {
					best_neighbor[i] = best_neighbor[best_neighbor[i]];
				}
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (!in_bsn[i]) continue;
			if (belonged[i] != i)
				best_neighbor[i] = best_neighbor[belonged[i]];
		}
		size_t idx_bsn = 0;
		std::unordered_map<size_t, size_t> center_to_id_bsn;
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (!in_bsn[i]) continue;
			if (center_to_id_bsn.count(best_neighbor[i]) == 0) {
				center_to_id_bsn[best_neighbor[i]] = idx_bsn++;
			}
		}
		std::map<size_t, size_t> aff_bsn;
		for (size_t id_ssp : m_info_bsns_atrct[id_bsn]->subspaces) {
			aff_bsn[id_ssp] = center_to_id_bsn[best_neighbor[belonged[id_ssp]]];
		}
		return aff_bsn;
	}

	size_t HLC::clustering() {
		/* merge adjacent subspaces of equal fit */
		std::vector<size_t> belonged(m_info_subspaces.size());
		std::vector<std::set<size_t>> neighbors(m_info_subspaces.size());
		std::vector<bool> visited(m_info_subspaces.size(), false);
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (!visited[i]) {
				belonged[i] = i;
				for (size_t nn : m_info_subspaces[i]->neighbors)
					neighbors[i].insert(nn);
				std::list<int> neis;
				neis.push_back(i);
				visited[i] = true;
				while (!neis.empty()) {
					int cur_id = neis.front();
					neis.pop_front();
					for (size_t n : m_info_subspaces[cur_id]->neighbors) {
						if (m_info_subspaces[cur_id]->obj == m_info_subspaces[n]->obj && !visited[n]) {
							belonged[n] = i;
							for (size_t nn : m_info_subspaces[n]->neighbors)
								neighbors[i].insert(nn);
							neis.push_back(n);
							visited[n] = true;
						}
					}
				}
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++)
			neighbors[belonged[i]].erase(i);
		std::vector<size_t> best_neighbor(m_info_subspaces.size());
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (belonged[i] != i) continue;
			best_neighbor[i] = i;
			for (size_t n : neighbors[i]) {
				if ((m_opt_mode_bsn_atrct[0] == OptMode::kMinimize && m_info_subspaces[best_neighbor[i]]->obj > m_info_subspaces[n]->obj)
					|| (m_opt_mode_bsn_atrct[0] == OptMode::kMaximize && m_info_subspaces[best_neighbor[i]]->obj < m_info_subspaces[n]->obj))
					best_neighbor[i] = belonged[n];
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (belonged[i] == i) {
				while (best_neighbor[best_neighbor[i]] != best_neighbor[i]) {
					best_neighbor[i] = best_neighbor[best_neighbor[i]];
				}
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (belonged[i] != i)
				best_neighbor[i] = best_neighbor[belonged[i]];
		}
		size_t id_bsn = 0;
		std::unordered_map<size_t, size_t> center_to_id_bsn;
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (center_to_id_bsn.count(best_neighbor[i]) == 0) {
				center_to_id_bsn[best_neighbor[i]] = id_bsn++;
			}
		}
		m_info_bsns_atrct.clear();
		m_info_bsns_atrct.resize(center_to_id_bsn.size());
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++)
			m_info_bsns_atrct[i].reset(new InfoBsnAtrct);
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			m_info_subspaces[i]->aff_bsn_atr = center_to_id_bsn[best_neighbor[belonged[i]]];
			auto &info_bsn = *m_info_bsns_atrct[m_info_subspaces[i]->aff_bsn_atr];
			info_bsn.seq_ssps[i] = info_bsn.subspaces.size();
			info_bsn.subspaces.push_back(i);
			info_bsn.vols_ssps.push_back(m_subspace_tree->getBoxVolume(i));
			info_bsn.volume += m_subspace_tree->getBoxVolume(i);
		}

		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			if (m_num_divs > 0)
				divideBsnAtrct(i);

			auto &info_bsn = *m_info_bsns_atrct[i];

			for (size_t id_ssp : info_bsn.subspaces) {
				if (!m_info_subspaces[id_ssp]->sols.empty()) {
					updateBsnAtrctInfo(i, m_info_subspaces[id_ssp]->best_sol);
					updateBsnAtrctInfo(i, m_info_subspaces[id_ssp]->worst_sol);
				}
			}

			info_bsn.prob_ssps = info_bsn.vols_ssps;
			for (size_t i = 0; i < info_bsn.vols_ssps.size(); i++)
				info_bsn.prob_ssps[i] /= pow(m_info_subspaces[info_bsn.subspaces[i]]->sols.size() + 1, 2);
			Real max_prob = info_bsn.prob_ssps.front();
			for (size_t i = 1; i < info_bsn.prob_ssps.size(); i++) {
				if (max_prob < info_bsn.prob_ssps[i])
					max_prob = info_bsn.prob_ssps[i];
			}
			for (size_t i = 0; i < info_bsn.prob_ssps.size(); i++)
				info_bsn.prob_ssps[i] = info_bsn.prob_ssps[i] / max_prob;

			info_bsn.cov_ssps.clear();
		}
 		return m_info_bsns_atrct.size();
	}

	void HLC::updatePotential() {
		/* update objective, frequency */
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			size_t ssp_best = m_info_bsns_atrct[i]->subspaces.front();
			m_info_bsns_atrct[i]->freq = 0;
			for (size_t ssp : m_info_bsns_atrct[i]->subspaces) {
				m_info_bsns_atrct[i]->freq += m_info_subspaces[ssp]->sols.size();
				if (!m_info_subspaces[ssp_best]->best_sol || (m_info_subspaces[ssp]->best_sol && m_info_subspaces[ssp]->best_sol->dominate(*m_info_subspaces[ssp_best]->best_sol, m_opt_mode_sol)))
					ssp_best = ssp;
			}
			m_info_bsns_atrct[i]->fit = m_info_subspaces[ssp_best]->obj;
		}

		if (m_info_bsns_atrct.size() == 1) {
			m_info_bsns_atrct[0]->potential = 1.0;
			return;
		}

		//std::vector<std::vector<Real>> stat_bsn(m_info_bsns_atrct.size(), std::vector<Real>(3));
		std::vector<std::vector<Real>> stat_bsn(m_info_bsns_atrct.size(), std::vector<Real>(2));

		/* normalize and discretize objective */
		Real pos = m_opt_mode_sol[0] == OptMode::kMaximize ? 1 : -1;
		Real min_obj = pos * m_worst_sol->objective(0);
		Real max_obj = pos * m_best_sol->objective(0);
		Real interval_obj = (max_obj - min_obj) / 100;
		Real obj_bsn;
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			obj_bsn = pos * m_info_bsns_atrct[i]->fit;
			stat_bsn[i][0] = ceil((obj_bsn - min_obj) / interval_obj) / 100;
		}

		/* normalize and discretize freq */
		Real min_freq = m_info_bsns_atrct[0]->freq;
		Real max_freq = m_info_bsns_atrct[0]->freq;
		for (size_t i = 1; i < m_info_bsns_atrct.size(); ++i) {
			if (min_freq > m_info_bsns_atrct[i]->freq)
				min_freq = m_info_bsns_atrct[i]->freq;
			if (max_freq < m_info_bsns_atrct[i]->freq)
				max_freq = m_info_bsns_atrct[i]->freq;
		}
		Real interval_freq = (max_freq - min_freq) / 100;
		Real freq_bsn;
		for (size_t i = 0; i < m_info_bsns_atrct.size(); ++i) {
			freq_bsn = m_info_bsns_atrct[i]->freq;
			stat_bsn[i][1] = ceil((freq_bsn - min_freq) / interval_freq) / 100;
		}

		/* update difficulty */
		//Real min_diff = m_info_bsns_atrct[0]->diff;
		//Real max_diff = m_info_bsns_atrct[0]->diff;
		//for (size_t i = 1; i < m_info_bsns_atrct.size(); ++i) {
		//	if (min_diff > m_info_bsns_atrct[i]->diff)
		//		min_diff = m_info_bsns_atrct[i]->diff;
		//	if (max_diff < m_info_bsns_atrct[i]->diff)
		//		max_diff = m_info_bsns_atrct[i]->diff;
		//}
		//Real interval_diff = (max_diff - min_diff) / 100;
		//Real diff_bsn;
		//for (size_t i = 0; i < m_info_bsns_atrct.size(); ++i) {
		//	diff_bsn = m_info_bsns_atrct[i]->diff;
		//	stat_bsn[i][2] = ceil((diff_bsn - min_diff) / interval_diff) / 100;
		//}

		std::vector<std::vector<Real>*> data(stat_bsn.size());
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++)
			data[i] = &stat_bsn[i];
		std::vector<int> rank;
		//std::vector<OptMode> opt_modes = { OptMode::Maximize, OptMode::kMinimize, OptMode::Maximize };
		std::vector<OptMode> opt_modes = { OptMode::kMaximize, OptMode::kMinimize };
		int num_rank = nd_sort::fastSort<Real>(data, rank, opt_modes);
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			m_info_bsns_atrct[i]->potential = 1. - Real(rank[i]) / num_rank;
		}
	}

	void HLC::updateBsnDiff(int id_pro) {
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			for (size_t id_ssp : m_info_bsns_atrct[i]->subspaces) {

			}
			std::vector<Solution<>> t_sols;
			DPC dpc;
			dpc.updateData(t_sols, id_pro);
			dpc.clustering();
		}
	}

	//void HLC::updatePotential(int id_pro, int id_rnd, const std::list<Solution<>> &attractors_fnd) {
	//	/* update fit */
	//	for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
	//		size_t ssp_best = m_info_bsns_atrct[i]->subspaces.front();
	//		m_info_bsns_atrct[i]->freq = 0;
	//		for (size_t ssp : m_info_bsns_atrct[i]->subspaces) {
	//			m_info_bsns_atrct[i]->freq += m_info_subspaces[ssp]->sols.size();
	//			if (!m_info_subspaces[ssp_best]->best_sol || (m_info_subspaces[ssp]->best_sol && m_info_subspaces[ssp]->best_sol->dominate(*m_info_subspaces[ssp_best]->best_sol, m_opt_mode_sol)))
	//				ssp_best = ssp;
	//		}
	//		m_info_bsns_atrct[i]->fit = m_info_subspaces[ssp_best]->obj;
	//	}
	//	Real pos = m_opt_mode_sol[0] == OptMode::Maximize ? 1 : -1;
	//	Real min_obj = pos * m_worst_sol->objective(0);
	//	Real max_obj = pos * m_best_sol->objective(0);
	//	Real interval = (max_obj - min_obj) / 10000;
	//	Real obj_bsn;
	//	for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
	//		obj_bsn = pos * m_info_bsns_atrct[i]->fit;
	//		m_info_bsns_atrct[i]->fit = floor((obj_bsn - min_obj) / interval) / 10000;
	//	}
	//	
	//	/* update freq */
	//	Real min_freq = m_info_bsns_atrct[0]->freq;
	//	Real max_freq = m_info_bsns_atrct[0]->freq;
	//	for (size_t i = 1; i < m_info_bsns_atrct.size(); ++i) {
	//		if (min_freq > m_info_bsns_atrct[i]->freq)
	//			min_freq = m_info_bsns_atrct[i]->freq;
	//		if (max_freq < m_info_bsns_atrct[i]->freq)
	//			max_freq = m_info_bsns_atrct[i]->freq;
	//	}
	//	for (size_t i = 0; i < m_info_bsns_atrct.size(); ++i)
	//		m_info_bsns_atrct[i]->freq = (max_freq - m_info_bsns_atrct[i]->freq) / (max_freq - min_freq);
	//	
	//	/* update difficulty */
	//	Real max_num_mdls = 1;
	//	for (size_t id_bsn = 0; id_bsn < m_info_bsns_atrct.size(); id_bsn++) {
	//		std::vector<const Solution<>*> his_sols;
	//		for (size_t id_ssp : m_info_bsns_atrct[id_bsn]->subspaces) {
	//			for (auto &sol : m_info_subspaces[id_ssp]->improved_sols)
	//				his_sols.push_back(sol.get());
	//		}
	//		auto &typ_sols = m_info_bsns_atrct[id_bsn]->typ_inds;
	//		if (his_sols.size() > 500) {
	//			GET_RND(id_rnd).uniform.shuffle(his_sols.begin(), his_sols.end());
	//			for (size_t i = 0; i < 500; ++i)
	//				typ_sols.emplace_back(new Individual<>(*his_sols[i]));
	//		}
	//		else
	//			for (size_t i = 0; i < his_sols.size(); ++i)
	//				typ_sols.emplace_back(new Individual<>(*his_sols[i]));
	//		
	//		int atrct_size = 0;
	//		for (auto &sol : attractors_fnd) {
	//			if (getIdxBsn(sol) == id_bsn)
	//				atrct_size++;
	//		}
	//		//if (m_opt_mode_sol[0] == OptMode::Maximize) {
	//		//	for (auto &it : typ_sols) {
	//		//		it->setFitness(1.0 / it->objective(0));
	//		//	}
	//		//}
	//		//else {
	//		//	for (auto &it : typ_sols) {
	//		//		it->setFitness(it->objective(0));
	//		//	}
	//		//}
	//		//FDC<Individual<>> fdc(typ_sols, id_pro);
	//		//fdc.calFitDist();
	//		//auto &obj_dis = fdc.getFitDist();
	//		//Real min_fit, max_fit, min_dist, max_dist;
	//		//min_fit = max_fit = obj_dis.front().first;
	//		//min_dist = max_dist = obj_dis.front().second;
	//		//for (size_t i = 1; i < obj_dis.size(); ++i) {
	//		//	if (obj_dis[i].first < min_fit) min_fit = obj_dis[i].first;
	//		//	if (obj_dis[i].first > max_fit) max_fit = obj_dis[i].first;
	//		//	if (obj_dis[i].second < min_dist) min_dist = obj_dis[i].second;
	//		//	if (obj_dis[i].second > max_dist) max_dist = obj_dis[i].second;
	//		//}
	//		//size_t clusters_size = 0;
	//		//for (size_t i = 0; i < obj_dis.size(); ++i) {
	//		//	if (obj_dis[i].first > min_fit + 0.8 * (max_fit - min_fit) &&
	//		//		obj_dis[i].second > min_dist + 0.1 * (max_dist - min_dist))
	//		//		clusters_size++;
	//		//}
	//		//HSLH<Individual<>> hslh(typ_sols, id_pro);
	//		//hslh.clustering(5, id_pro);
	//		//size_t clusters_size = hslh.clusters().size();
	//		DPC dpc;
	//		dpc.updateData(typ_sols, id_pro);
	//		dpc.clustering(0.1, 0.1);
	//		m_info_bsns_atrct[id_bsn]->rho = dpc.getRho();
	//		m_info_bsns_atrct[id_bsn]->delta = dpc.getDelta();
	//		int clusters_size = dpc.clusters().size();
	//		m_info_bsns_atrct[id_bsn]->diff = clusters_size - atrct_size;
	//		if (m_info_bsns_atrct[id_bsn]->diff > max_num_mdls)
	//			max_num_mdls = m_info_bsns_atrct[id_bsn]->diff;
	//	}
	//	for (size_t id_bsn = 0; id_bsn < m_info_bsns_atrct.size(); id_bsn++)
	//		m_info_bsns_atrct[id_bsn]->diff /= max_num_mdls;
	//	 
	//	/* update potential */
	//	for (size_t id_bsn = 0; id_bsn < m_info_bsns_atrct.size(); ++id_bsn)
	//		m_info_bsns_atrct[id_bsn]->potential =
	//		0.3 * m_info_bsns_atrct[id_bsn]->fit
	//		+ 0.2 * m_info_bsns_atrct[id_bsn]->freq
	//		+ 0.5 * m_info_bsns_atrct[id_bsn]->diff;
	//	//std::vector<std::vector<Real> *> data(stat_basn.size());
	//	//for (size_t i = 0; i < stat_basn.size(); i++)
	//	//	data[i] = &stat_basn[i];
	//	//std::vector<int> rank;
	//	//int num_rank = NS::fast_sort<Real>(data, rank, m_opt_mode_bsn_atrct);
	//	//for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
	//	//	m_info_bsns_atrct[i].potential = 1. - Real(rank[i]) / num_rank;
	//	//}
	//}

	void HLC::updateDPC(size_t id_bsn, const std::vector<const Solution<>*> &sols, int id_pro, int id_rnd) {
		auto &typ_sols = m_info_bsns_atrct[id_bsn]->typ_inds;
		for (size_t i = 0; i < sols.size(); ++i)
			typ_sols.emplace_back(new Individual<>(*sols[i]));
		DPC dpc;
		dpc.updateData(typ_sols, id_pro);
		dpc.clustering(0.01,0.3);
		m_info_bsns_atrct[id_bsn]->rho = dpc.getRho();
		m_info_bsns_atrct[id_bsn]->delta = dpc.getDelta();
	}

	void HLC::clearTypInds() {
		for (size_t id_bsn = 0; id_bsn < m_info_bsns_atrct.size(); ++id_bsn)
			m_info_bsns_atrct[id_bsn]->typ_inds.clear();
	}

	//void HLC::splitBasin(size_t id_basin, size_t num_parts) {
	//	auto &info = *m_info_bsns_atrct[id_basin];
	//	while (info.subspaces.size() < num_parts) {
	//		size_t largest = info.subspaces.front();
	//		for (size_t i = 1; i < info.subspaces.size(); ++i) {
	//			if (m_subspace_tree->getBoxVolume(info.subspaces[i]) > m_subspace_tree->getBoxVolume(largest))
	//				largest = info.subspaces[i];
	//		}
	//		size_t added = m_subspace_tree->splitRegion(largest);
	//		info.subspaces.push_back(added);
	//		/* solution */
	//		m_info_subspaces.emplace_back(new InfoSubspace);
	//		m_info_subspaces[added]->aff_bsn_atr = m_info_subspaces[largest]->aff_bsn_atr;
	//		auto sols = m_info_subspaces[largest]->sols;
	//		resetSubspaceInfo(largest);
	//		resetSubspaceInfo(added);
	//		size_t id_ssp;
	//		for (auto &ptr_sol : sols) {
	//			id_ssp = m_subspace_tree->getRegionIdx(ptr_sol->variable().vect());
	//			updateSubspaceInfo(id_ssp, ptr_sol);
	//		}
	//		/* neighbors */
	//		m_info_subspaces[added]->neighbors.clear();
	//		for (auto iter = m_info_subspaces[largest]->neighbors.begin(); iter != m_info_subspaces[largest]->neighbors.end();) {
	//			if (isAdjacent(m_subspace_tree->getBox(added), m_subspace_tree->getBox(*iter))) {
	//				m_info_subspaces[added]->neighbors.push_back(*iter);
	//				m_info_subspaces[*iter]->neighbors.push_back(added);
	//			}
	//			if (!isAdjacent(m_subspace_tree->getBox(largest), m_subspace_tree->getBox(*iter))) {
	//				m_info_subspaces[*iter]->neighbors.erase(std::find(m_info_subspaces[*iter]->neighbors.begin(), m_info_subspaces[*iter]->neighbors.end(), largest));
	//				iter = m_info_subspaces[largest]->neighbors.erase(iter);
	//			}
	//			else
	//				iter++;
	//		}
	//		m_info_subspaces[largest]->neighbors.push_back(added);
	//		m_info_subspaces[added]->neighbors.push_back(largest);
	//	}
	//	/* Basin */
	//	info.vols_ssps.resize(info.subspaces.size());
	//	for (size_t i = 0; i < info.subspaces.size(); i++)
	//		info.vols_ssps[i] = m_subspace_tree->getBoxVolume(info.subspaces[i]);
	//	info.prob_ssps = info.vols_ssps;
	//	for (size_t i = 0; i < info.vols_ssps.size(); i++)
	//		info.prob_ssps[i] /= pow(m_info_subspaces[info.subspaces[i]]->sols.size() + 1, 2);
	//	Real max_prob = info.prob_ssps.front();
	//	for (size_t i = 1; i < info.prob_ssps.size(); i++) {
	//		if (max_prob < info.prob_ssps[i])
	//			max_prob = info.prob_ssps[i];
	//	}
	//	for (size_t i = 0; i < info.prob_ssps.size(); i++)
	//		info.prob_ssps[i] = info.prob_ssps[i] / max_prob;
	//	info.seq_ssps.clear();
	//	for (size_t i = 0; i < info.subspaces.size(); i++) {
	//		info.seq_ssps[info.subspaces[i]] = i;
	//	}
	//}

	//void HLC::splitBasin(size_t id_bsn_atrct, const std::vector<const Solution<> *> &atrctrs) {
	//	std::list<std::array<size_t, 3>> in_same_ssp;    // [0][1]: indexes of atrctrs, [2]: index of subspace
	//	std::list<std::array<size_t, 4>> in_adjcnt_ssp;  // [0][1]: indexes of atrctrs, [2][3]: indexes of subspaces
	//	for (size_t i = 0; i < atrctrs.size(); i++) {
	//		size_t id_ssp_i = getIdxSSP(*atrctrs[i]);
	//		for (size_t j = i + 1; j < atrctrs.size(); j++) {
	//			size_t id_ssp_j = getIdxSSP(*atrctrs[j]);
	//			if (id_ssp_i == id_ssp_j)
	//				in_same_ssp.push_back({ i, j, id_ssp_i });
	//			else if (isAdjacent(m_subspace_tree->getBox(id_ssp_i), m_subspace_tree->getBox(id_ssp_j)))
	//				in_adjcnt_ssp.push_back({ i, j, id_ssp_i, id_ssp_j });
	//		}
	//	}
	//	size_t added_1, added_2;
	//	while (!in_same_ssp.empty()) {
	//		int dim_split = 0;
	//		Real pivot_1, pivot_2;
	//		size_t a1 = in_same_ssp.front()[0], a2 = in_same_ssp.front()[1], ssp = in_same_ssp.front()[2];
	//		Real diff, diff_split = abs(atrctrs[a1]->variable()[dim_split] - atrctrs[a2]->variable()[dim_split]);
	//		for (size_t j = 1; j < m_size_var; ++j) {
	//			diff = abs(atrctrs[a1]->variable()[j] - atrctrs[a2]->variable()[j]);
	//			if (diff > diff_split) {
	//				diff_split = diff;
	//				dim_split = j;
	//			}
	//		}
	//		pivot_1 = (atrctrs[a1]->variable()[dim_split] + 2 * atrctrs[a2]->variable()[dim_split]) / 3;
	//		pivot_2 = (atrctrs[a2]->variable()[dim_split] + 2 * atrctrs[a1]->variable()[dim_split]) / 3;
	//		added_1 = m_subspace_tree->splitRegion(ssp, &dim_split, &pivot_1);
	//		updateInfoAfterSplitSSP(ssp, added_1, id_bsn_atrct);
	//		if (m_subspace_tree->getBox(ssp)[dim_split].first < pivot_2 && pivot_2 < m_subspace_tree->getBox(ssp)[dim_split].second) {
	//			added_2 = m_subspace_tree->splitRegion(ssp, &dim_split, &pivot_2);
	//			updateInfoAfterSplitSSP(ssp, added_2, id_bsn_atrct);
	//		}
	//		else {
	//			added_2 = m_subspace_tree->splitRegion(added_1, &dim_split, &pivot_2);
	//			updateInfoAfterSplitSSP(added_1, added_2, id_bsn_atrct);
	//		}
	//		in_same_ssp.pop_front();
	//		for (auto iter = in_same_ssp.begin(); iter != in_same_ssp.end();) {
	//			size_t id_ssp_1 = getIdxSSP(*atrctrs[(*iter)[0]]);
	//			size_t id_ssp_2 = getIdxSSP(*atrctrs[(*iter)[1]]);
	//			if (id_ssp_1 != id_ssp_2)
	//				iter = in_same_ssp.erase(iter);
	//			else {
	//				(*iter)[2] = id_ssp_1;
	//				iter++;
	//			}
	//		}
	//		for (auto iter = in_adjcnt_ssp.begin(); iter != in_adjcnt_ssp.end();) {
	//			size_t id_ssp_1 = getIdxSSP(*atrctrs[(*iter)[0]]);
	//			size_t id_ssp_2 = getIdxSSP(*atrctrs[(*iter)[1]]);
	//			if (!isAdjacent(m_subspace_tree->getBox(id_ssp_1), m_subspace_tree->getBox(id_ssp_2)))
	//				iter = in_adjcnt_ssp.erase(iter);
	//			else {
	//				(*iter)[2] = id_ssp_1;
	//				(*iter)[3] = id_ssp_2;
	//				iter++;
	//			}
	//		}
	//	}
	//	while (!in_adjcnt_ssp.empty()) {
	//		int dim_split = 0;
	//		Real pivot;
	//		Real division;
	//		size_t a1 = in_adjcnt_ssp.front()[0], a2 = in_adjcnt_ssp.front()[1];
	//		size_t ssp1 = in_adjcnt_ssp.front()[2], ssp2 = in_adjcnt_ssp.front()[3];
	//		for (size_t j = 0; j < m_size_var; ++j) {
	//			auto &box1 = m_subspace_tree->getBox(ssp1);
	//			auto &box2 = m_subspace_tree->getBox(ssp2);
	//			if (box1[j].first == box2[j].second) {
	//				division = box1[j].first;
	//				dim_split = j;
	//				break;
	//			}
	//			if (box1[j].second == box2[j].first) {
	//				division = box1[j].second;
	//				dim_split = j;
	//				break;
	//			}
	//		}
	//		Real diff1 = abs(atrctrs[a1]->variable()[dim_split] - division);
	//		Real diff2 = abs(atrctrs[a2]->variable()[dim_split] - division);
	//		if (diff1 > diff2) {
	//			pivot = (atrctrs[a1]->variable()[dim_split] + division) / 2;
	//			added_1 = m_subspace_tree->splitRegion(ssp1, &dim_split, &pivot);
	//			updateInfoAfterSplitSSP(ssp1, added_1, id_bsn_atrct);
	//		}
	//		else {
	//			pivot = (atrctrs[a2]->variable()[dim_split] + division) / 2;
	//			added_1 = m_subspace_tree->splitRegion(ssp2, &dim_split, &pivot);
	//			updateInfoAfterSplitSSP(ssp2, added_1, id_bsn_atrct);
	//		}
	//		in_adjcnt_ssp.pop_front();
	//		for (auto iter = in_adjcnt_ssp.begin(); iter != in_adjcnt_ssp.end();) {
	//			size_t id_ssp_1 = getIdxSSP(*atrctrs[(*iter)[0]]);
	//			size_t id_ssp_2 = getIdxSSP(*atrctrs[(*iter)[1]]);
	//			if (!isAdjacent(m_subspace_tree->getBox(id_ssp_1), m_subspace_tree->getBox(id_ssp_2)))
	//				iter = in_adjcnt_ssp.erase(iter);
	//			else {
	//				(*iter)[2] = id_ssp_1;
	//				(*iter)[3] = id_ssp_2;
	//				iter++;
	//			}
	//		}
	//	}
	//	/* Basin */
	//	auto &info = *m_info_bsns_atrct[id_bsn_atrct];
	//	info.vols_ssps.resize(info.subspaces.size());
	//	for (size_t i = 0; i < info.subspaces.size(); i++)
	//		info.vols_ssps[i] = m_subspace_tree->getBoxVolume(info.subspaces[i]);
	//	info.prob_ssps = info.vols_ssps;
	//	for (size_t i = 0; i < info.vols_ssps.size(); i++)
	//		info.prob_ssps[i] /= pow(m_info_subspaces[info.subspaces[i]]->sols.size() + 1, 2);
	//	Real max_prob = info.prob_ssps.front();
	//	for (size_t i = 1; i < info.prob_ssps.size(); i++) {
	//		if (max_prob < info.prob_ssps[i])
	//			max_prob = info.prob_ssps[i];
	//	}
	//	for (size_t i = 0; i < info.prob_ssps.size(); i++)
	//		info.prob_ssps[i] = info.prob_ssps[i] / max_prob;
	//	info.seq_ssps.clear();
	//	for (size_t i = 0; i < info.subspaces.size(); i++) {
	//		info.seq_ssps[info.subspaces[i]] = i;
	//	}
	//}

	//void HLC::splitSubspaces(const std::vector<const Solution<> *> &atrctrs) {
	//	std::list<std::array<size_t, 3>> in_same_ssp;    // [0][1]: indexes of atrctrs, [2]: index of subspace
	//	std::list<std::array<size_t, 4>> in_adjcnt_ssp;  // [0][1]: indexes of atrctrs, [2][3]: indexes of subspaces
	//	for (size_t i = 0; i < atrctrs.size(); i++) {
	//		size_t id_ssp_i = getIdxSSP(*atrctrs[i]);
	//		for (size_t j = i + 1; j < atrctrs.size(); j++) {
	//			size_t id_ssp_j = getIdxSSP(*atrctrs[j]);
	//			if (id_ssp_i == id_ssp_j)
	//				in_same_ssp.push_back({ i, j, id_ssp_i });
	//			else if (isAdjacent(m_subspace_tree->getBox(id_ssp_i), m_subspace_tree->getBox(id_ssp_j)))
	//				in_adjcnt_ssp.push_back({ i, j, id_ssp_i, id_ssp_j });
	//		}
	//	}
	//	size_t added_1, added_2;
	//	while (!in_same_ssp.empty()) {
	//		int dim_split = 0;
	//		Real pivot_1, pivot_2;
	//		size_t a1 = in_same_ssp.front()[0], a2 = in_same_ssp.front()[1], ssp = in_same_ssp.front()[2];
	//		Real diff, diff_split = abs(atrctrs[a1]->variable()[dim_split] - atrctrs[a2]->variable()[dim_split]);
	//		for (size_t j = 1; j < m_size_var; ++j) {
	//			diff = abs(atrctrs[a1]->variable()[j] - atrctrs[a2]->variable()[j]);
	//			if (diff > diff_split) {
	//				diff_split = diff;
	//				dim_split = j;
	//			}
	//		}
	//		pivot_1 = (atrctrs[a1]->variable()[dim_split] + 2 * atrctrs[a2]->variable()[dim_split]) / 3;
	//		pivot_2 = (atrctrs[a2]->variable()[dim_split] + 2 * atrctrs[a1]->variable()[dim_split]) / 3;
	//		added_1 = m_subspace_tree->splitRegion(ssp, &dim_split, &pivot_1);
	//		updateInfoAfterSplitSSP(ssp, added_1);
	//		if (m_subspace_tree->getBox(ssp)[dim_split].first < pivot_2 && pivot_2 < m_subspace_tree->getBox(ssp)[dim_split].second) {
	//			added_2 = m_subspace_tree->splitRegion(ssp, &dim_split, &pivot_2);
	//			updateInfoAfterSplitSSP(ssp, added_2);
	//		}
	//		else {
	//			added_2 = m_subspace_tree->splitRegion(added_1, &dim_split, &pivot_2);
	//			updateInfoAfterSplitSSP(added_1, added_2);
	//		}
	//		in_same_ssp.pop_front();
	//		for (auto iter = in_same_ssp.begin(); iter != in_same_ssp.end();) {
	//			size_t id_ssp_1 = getIdxSSP(*atrctrs[(*iter)[0]]);
	//			size_t id_ssp_2 = getIdxSSP(*atrctrs[(*iter)[1]]);
	//			if (id_ssp_1 != id_ssp_2) {
	//				if (isAdjacent(m_subspace_tree->getBox(id_ssp_1), m_subspace_tree->getBox(id_ssp_2)))
	//					in_adjcnt_ssp.push_back({ (*iter)[0], (*iter)[1], id_ssp_1, id_ssp_2 });
	//				iter = in_same_ssp.erase(iter);
	//			}
	//			else {
	//				(*iter)[2] = id_ssp_1;
	//				iter++;
	//			}
	//		}
	//		for (auto iter = in_adjcnt_ssp.begin(); iter != in_adjcnt_ssp.end();) {
	//			size_t id_ssp_1 = getIdxSSP(*atrctrs[(*iter)[0]]);
	//			size_t id_ssp_2 = getIdxSSP(*atrctrs[(*iter)[1]]);
	//			if (!isAdjacent(m_subspace_tree->getBox(id_ssp_1), m_subspace_tree->getBox(id_ssp_2)))
	//				iter = in_adjcnt_ssp.erase(iter);
	//			else {
	//				(*iter)[2] = id_ssp_1;
	//				(*iter)[3] = id_ssp_2;
	//				iter++;
	//			}
	//		}
	//	}
	//	while (!in_adjcnt_ssp.empty()) {
	//		int dim_split = 0;
	//		Real pivot;
	//		Real division;
	//		size_t a1 = in_adjcnt_ssp.front()[0], a2 = in_adjcnt_ssp.front()[1];
	//		size_t ssp1 = in_adjcnt_ssp.front()[2], ssp2 = in_adjcnt_ssp.front()[3];
	//		for (size_t j = 0; j < m_size_var; ++j) {
	//			auto &box1 = m_subspace_tree->getBox(ssp1);
	//			auto &box2 = m_subspace_tree->getBox(ssp2);
	//			if (box1[j].first == box2[j].second) {
	//				division = box1[j].first;
	//				dim_split = j;
	//				break;
	//			}
	//			if (box1[j].second == box2[j].first) {
	//				division = box1[j].second;
	//				dim_split = j;
	//				break;
	//			}
	//		}
	//		Real diff1 = abs(atrctrs[a1]->variable()[dim_split] - division);
	//		Real diff2 = abs(atrctrs[a2]->variable()[dim_split] - division);
	//		if (diff1 > diff2) {
	//			pivot = (atrctrs[a1]->variable()[dim_split] + division) / 2;
	//			added_1 = m_subspace_tree->splitRegion(ssp1, &dim_split, &pivot);
	//			updateInfoAfterSplitSSP(ssp1, added_1);
	//		}
	//		else {
	//			pivot = (atrctrs[a2]->variable()[dim_split] + division) / 2;
	//			added_1 = m_subspace_tree->splitRegion(ssp2, &dim_split, &pivot);
	//			updateInfoAfterSplitSSP(ssp2, added_1);
	//		}
	//		in_adjcnt_ssp.pop_front();
	//		for (auto iter = in_adjcnt_ssp.begin(); iter != in_adjcnt_ssp.end();) {
	//			size_t id_ssp_1 = getIdxSSP(*atrctrs[(*iter)[0]]);
	//			size_t id_ssp_2 = getIdxSSP(*atrctrs[(*iter)[1]]);
	//			if (!isAdjacent(m_subspace_tree->getBox(id_ssp_1), m_subspace_tree->getBox(id_ssp_2)))
	//				iter = in_adjcnt_ssp.erase(iter);
	//			else {
	//				(*iter)[2] = id_ssp_1;
	//				(*iter)[3] = id_ssp_2;
	//				iter++;
	//			}
	//		}
	//	}
	//}

	//void HLC::updateInfoAfterSplitSSP(size_t old_ssp, size_t new_ssp, size_t id_bsn) {
	//	/* basin */
	//	m_info_bsns_atrct[id_bsn]->subspaces.push_back(new_ssp);
	//	/* solution */
	//	m_info_subspaces.emplace_back(new InfoSubspace);
	//	m_info_subspaces[new_ssp]->aff_bsn_atr = m_info_subspaces[old_ssp]->aff_bsn_atr;
	//	auto sols = m_info_subspaces[old_ssp]->sols;
	//	resetSubspaceInfo(old_ssp);
	//	resetSubspaceInfo(new_ssp);
	//	size_t id_ssp;
	//	for (auto &ptr_sol : sols) {
	//		id_ssp = m_subspace_tree->getRegionIdx(ptr_sol->variable().vect());
	//		updateSubspaceInfo(id_ssp, ptr_sol);
	//	}
	//	/* neighbors */
	//	m_info_subspaces[new_ssp]->neighbors.clear();
	//	for (auto iter = m_info_subspaces[old_ssp]->neighbors.begin(); iter != m_info_subspaces[old_ssp]->neighbors.end();) {
	//		if (isAdjacent(m_subspace_tree->getBox(new_ssp), m_subspace_tree->getBox(*iter))) {
	//			m_info_subspaces[new_ssp]->neighbors.push_back(*iter);
	//			m_info_subspaces[*iter]->neighbors.push_back(new_ssp);
	//		}
	//		if (!isAdjacent(m_subspace_tree->getBox(old_ssp), m_subspace_tree->getBox(*iter))) {
	//			m_info_subspaces[*iter]->neighbors.erase(std::find(m_info_subspaces[*iter]->neighbors.begin(), m_info_subspaces[*iter]->neighbors.end(), old_ssp));
	//			iter = m_info_subspaces[old_ssp]->neighbors.erase(iter);
	//		}
	//		else
	//			iter++;
	//	}
	//	m_info_subspaces[old_ssp]->neighbors.push_back(new_ssp);
	//	m_info_subspaces[new_ssp]->neighbors.push_back(old_ssp);
	//}

	//void HLC::updateInfoAfterSplitSSP(size_t old_ssp, size_t new_ssp) {
	//	/* solution */
	//	m_info_subspaces.emplace_back(new InfoSubspace);
	//	m_info_subspaces[new_ssp]->aff_bsn_atr = m_info_subspaces[old_ssp]->aff_bsn_atr;
	//	auto sols = m_info_subspaces[old_ssp]->sols;
	//	resetSubspaceInfo(old_ssp);
	//	resetSubspaceInfo(new_ssp);
	//	size_t id_ssp;
	//	for (auto &ptr_sol : sols) {
	//		id_ssp = m_subspace_tree->getRegionIdx(ptr_sol->variable().vect());
	//		updateSubspaceInfo(id_ssp, ptr_sol);
	//	}
	//	auto improved_sols = m_info_subspaces[old_ssp]->improved_sols;
	//	m_info_subspaces[old_ssp]->improved_sols.clear();
	//	for (auto &ptr_sol : improved_sols) {
	//		id_ssp = m_subspace_tree->getRegionIdx(ptr_sol->variable().vect());
	//		m_info_subspaces[id_ssp]->improved_sols.push_back(ptr_sol);
	//	}
	//	/* neighbors */
	//	m_info_subspaces[new_ssp]->neighbors.clear();
	//	for (auto iter = m_info_subspaces[old_ssp]->neighbors.begin(); iter != m_info_subspaces[old_ssp]->neighbors.end();) {
	//		if (isAdjacent(m_subspace_tree->getBox(new_ssp), m_subspace_tree->getBox(*iter))) {
	//			m_info_subspaces[new_ssp]->neighbors.push_back(*iter);
	//			m_info_subspaces[*iter]->neighbors.push_back(new_ssp);
	//		}
	//		if (!isAdjacent(m_subspace_tree->getBox(old_ssp), m_subspace_tree->getBox(*iter))) {
	//			m_info_subspaces[*iter]->neighbors.erase(std::find(m_info_subspaces[*iter]->neighbors.begin(), m_info_subspaces[*iter]->neighbors.end(), old_ssp));
	//			iter = m_info_subspaces[old_ssp]->neighbors.erase(iter);
	//		}
	//		else
	//			iter++;
	//	}
	//	m_info_subspaces[old_ssp]->neighbors.push_back(new_ssp);
	//	m_info_subspaces[new_ssp]->neighbors.push_back(old_ssp);
	//}

	void HLC::calCoverage(size_t id_basin, const std::vector<const Solution<> *> &sols, int id_pro, Real niche_radius) {
		std::vector<const Solution<> *> uni_inds;
		for (size_t i = 0; i < sols.size(); i++) {
			bool same = false;
			for (auto uni_ind : uni_inds) {
				if (uni_ind->variableDistance(*sols[i], id_pro) < niche_radius) {
					same = true;
					break;
				}
			}
			if (!same)
				uni_inds.push_back(sols[i]);
		}
		std::unordered_set<size_t> filled_divs;
		for (auto ptr_sol : uni_inds) {
			size_t aff_ssp = m_subspace_tree->getRegionIdx(ptr_sol->variable().vect());
			int aff_asp = -1;
			if (m_info_subspaces[aff_ssp]->atomspace_tree) {
				aff_asp = m_info_subspaces[aff_ssp]->atomspace_tree->getRegionIdx(ptr_sol->variable().vect());
			}
			size_t aff_div = m_info_bsns_atrct[id_basin]->ssp_atp_to_div[aff_ssp][aff_asp];
			filled_divs.insert(aff_div);
		}
		//Real filled_vol = 0;
		//for (size_t id_div : filled_divs)
		//	filled_vol += m_info_bsns_atrct[id_basin].vols_divs[id_div];
		//Real cov_by_vol = filled_vol / m_info_bsns_atrct[id_basin].volume;
		Real cov_by_num = (Real)filled_divs.size() / m_num_divs;
		m_info_bsns_atrct[id_basin]->coverages.push_back(cov_by_num);
		//m_info_bsns_atrct[id_basin].coverages.push_back(cov_by_vol < cov_by_num ? cov_by_vol : cov_by_num);
		if (m_info_bsns_atrct[id_basin]->cuml_covs.empty())
			m_info_bsns_atrct[id_basin]->cuml_covs.push_back(m_info_bsns_atrct[id_basin]->coverages.back());
		else
			m_info_bsns_atrct[id_basin]->cuml_covs.push_back(m_info_bsns_atrct[id_basin]->cuml_covs.back() + m_info_bsns_atrct[id_basin]->coverages.back());
	}

	void HLC::recordCovDivs(size_t id_bsn_atrct, const std::vector<const Solution<> *> &sols) {
		auto &info = *m_info_bsns_atrct[id_bsn_atrct];
		for (size_t i = 0; i < m_num_divs; i++)
			info.cov_divs[i].push_back(0);
		for (auto &sol : sols) {
			auto id_div = getIdxDiv(id_bsn_atrct, *sol);
			if (id_div > -1)
				info.cov_divs[id_div].back()++;
		}
		bool unchanged = true;
		size_t size_covs;
		for (size_t i = 0; i < m_num_divs; i++) {
			size_covs = info.cov_divs[i].size();
			if (size_covs == 1 || info.cov_divs[i][size_covs - 1] != info.cov_divs[i][size_covs - 2]) {
				unchanged = false;
				break;
			}
		}
		if (unchanged)
			info.num_cov_divs_remain_same++;
		else
			info.num_cov_divs_remain_same = 0;
	}

	void HLC::recordCovSSPs(size_t id_bsn_atrct, const std::vector<const Solution<> *> &sols) {
		auto &info = *m_info_bsns_atrct[id_bsn_atrct];
		size_t pre_num_cov = 0, cur_num_cov = 0;
		if (!info.cov_ssps.empty()) {
			for (size_t id_ssp : info.subspaces) {
				if (info.cov_ssps[id_ssp].back() > 0)
					pre_num_cov++;
			}
		}
		for (size_t id_ssp : info.subspaces)
			info.cov_ssps[id_ssp].push_back(0);
		for (auto &sol : sols)
			info.cov_ssps[getIdxSSP(*sol)].back()++;
		for (size_t id_ssp : info.subspaces) {
			if (info.cov_ssps[id_ssp].back() > 0)
				cur_num_cov++;
		}
		if (pre_num_cov == cur_num_cov)
			info.num_covered_ssps_remain++;
		else
			info.num_covered_ssps_remain = 0;
	}

	size_t HLC::numDivsFilled(size_t id_bsn_atrct, const std::vector<const Solution<> *> &sols) {
		std::unordered_set<size_t> filled_divs;
		for (auto ptr_sol : sols) {
			size_t aff_ssp = m_subspace_tree->getRegionIdx(ptr_sol->variable().vect());
			int aff_asp = -1;
			if (m_info_subspaces[aff_ssp]->atomspace_tree) {
				aff_asp = m_info_subspaces[aff_ssp]->atomspace_tree->getRegionIdx(ptr_sol->variable().vect());
			}
			size_t aff_div = m_info_bsns_atrct[id_bsn_atrct]->ssp_atp_to_div[aff_ssp][aff_asp];
			filled_divs.insert(aff_div);
		}
		return filled_divs.size();
	}

	void HLC::randomSmpl(size_t id_bsn_atrct, size_t id_div, size_t id_rnd, std::vector<Real> &smpl) {
		auto &division = m_info_bsns_atrct[id_bsn_atrct]->div_to_ssp_atp[id_div];
		auto rnd_div = GET_RND(id_rnd).uniform.nextElem(division.begin(), division.end());
		auto box(m_subspace_tree->getBox(rnd_div->first));
		if (rnd_div->second > -1)
			box = m_info_subspaces[rnd_div->first]->atomspace_tree->getBox(rnd_div->second);
		for (size_t j = 0; j < m_size_var; j++)
			smpl[j] = GET_RND(id_rnd).uniform.nextNonStd(box[j].first, box[j].second);
	}

	void HLC::randomSmpl(size_t id_bsn_atrct, size_t id_rnd, std::vector<Real> &smpl) {
		auto &info = *m_info_bsns_atrct[id_bsn_atrct];
		std::vector<Real> prob(info.vols_ssps);
		auto id_ssp = info.subspaces[GET_RND(id_rnd).uniform.spinWheel(prob.begin(), prob.end()) - prob.begin()];
		auto &box(m_subspace_tree->getBox(id_ssp));
		for (size_t j = 0; j < m_size_var; j++)
			smpl[j] = GET_RND(id_rnd).uniform.nextNonStd(box[j].first, box[j].second);
	}

	void HLC::randomSmplInSSP(size_t id_subspace, size_t id_rnd, std::vector<Real> &smpl) {
		auto &box(m_subspace_tree->getBox(id_subspace));
		for (size_t j = 0; j < m_size_var; j++)
			smpl[j] = GET_RND(id_rnd).uniform.nextNonStd(box[j].first, box[j].second);
	}

	void HLC::randomSmplByFreq(size_t id_bsn_atrct, size_t id_rnd, std::vector<Real> &smpl) {
		auto &info = *m_info_bsns_atrct[id_bsn_atrct];
		std::vector<Real> prob(info.vols_divs);
		for (size_t i = 0; i < info.vols_divs.size(); i++)
			prob[i] /= (info.freq_divs[i] + 1);
		int id_div = GET_RND(id_rnd).uniform.spinWheel(prob.begin(), prob.end()) - prob.begin();
		randomSmpl(id_bsn_atrct, id_div, id_rnd, smpl);
		//std::vector<Real> prob(info.vols_ssps);
		//for (size_t i = 0; i < info.vols_ssps.size(); i++)
		//	prob[i] /= pow(m_info_subspaces[info.subspaces[i]].freq + 1, 2);
		//auto id_ssp = info.subspaces[GET_RND(id_rnd).uniform.spinWheel(prob.begin(), prob.end()) - prob.begin()];
		//auto id_ssp = info.subspaces[GET_RND(id_rnd).uniform.spinWheel(info.prob_ssps.begin(), info.prob_ssps.end())
		//	- info.prob_ssps.begin()];
		//auto &box(m_subspace_tree->getBox(id_ssp));
		//for (size_t j = 0; j < m_size_var; j++)
		//	smpl[j] = GET_RND(id_rnd).uniform.nextNonStd(box[j].first, box[j].second);
	}

	int HLC::getIdxDiv(int id_bsn_atr, const Solution<> &sol) const {
		size_t id_ssp = m_subspace_tree->getRegionIdx(sol.variable().vect());
		if (m_info_subspaces[id_ssp]->aff_bsn_atr != id_bsn_atr)
			return -1;
		int id_asp = -1;
		if (m_info_subspaces[id_ssp]->atomspace_tree)
			id_asp = m_info_subspaces[id_ssp]->atomspace_tree->getRegionIdx(sol.variable().vect());
		return m_info_bsns_atrct[id_bsn_atr]->ssp_atp_to_div.at(id_ssp).at(id_asp);
	}

	int HLC::getIdxBsn(const Solution<> &sol) const {
		int id_ssp = m_subspace_tree->getRegionIdx(sol.variable().vect());
		if (id_ssp > -1 && id_ssp < m_info_subspaces.size())
			return m_info_subspaces[id_ssp]->aff_bsn_atr;
		else
			return -1;
	}

	int HLC::getIdxSSP(const Solution<> &sol) const {
		return m_subspace_tree->getRegionIdx(sol.variable().vect());
	}

	void HLC::calRuggedness(size_t id_bsn_atrct, int id_rnd, int id_pro, int id_alg) {
		auto &info = *m_info_bsns_atrct[id_bsn_atrct];
		size_t id = 0;
		auto worst_sol = m_info_subspaces[info.subspaces[id]]->worst_sol;
		auto best_sol = m_info_subspaces[info.subspaces[id++]]->best_sol;
		while (!worst_sol) {
			worst_sol = m_info_subspaces[info.subspaces[id]]->worst_sol;
			best_sol = m_info_subspaces[info.subspaces[id++]]->best_sol;
		}
		while (id < info.subspaces.size()) {
			if (!m_info_subspaces[info.subspaces[id]]->sols.empty()) {
				if (worst_sol->dominate(*m_info_subspaces[info.subspaces[id]]->worst_sol, m_opt_mode_sol))
					worst_sol = m_info_subspaces[info.subspaces[id]]->worst_sol;
				if (m_info_subspaces[info.subspaces[id]]->best_sol->dominate(*best_sol, m_opt_mode_sol))
					best_sol = m_info_subspaces[info.subspaces[id]]->best_sol;
			}
			id++;
		}
		Real max_obj_dif = best_sol->objectiveDistance(*worst_sol);
		info.ruggedness = 0;
		for (size_t id_ssp : info.subspaces) {
			if (m_info_subspaces[id_ssp]->sols.empty())
				continue;
			Solution<> tmp_sol(*m_info_subspaces[id_ssp]->best_sol);
			auto &box = m_subspace_tree->getBox(id_ssp);
			for (size_t j = 0; j < m_size_var; ++j) {
				tmp_sol.variable()[j] = GET_RND(id_rnd).normal.nextNonStd(tmp_sol.variable()[j], (box[j].second - box[j].first) / 1000);
			}
			tmp_sol.evaluate(id_pro, id_alg);
			info.ruggedness += tmp_sol.objectiveDistance(*m_info_subspaces[id_ssp]->best_sol) / max_obj_dif;
		}
		info.ruggedness /= info.subspaces.size();
	}

	std::list<size_t> HLC::divsConverge(size_t id_bsn_atrct) const {
		const auto &info = m_info_bsns_atrct[id_bsn_atrct];
		std::list<size_t> divs_converge;
		for (size_t i = 0; i < m_num_divs; i++) {
			if (info->cov_divs[i].back() > 0)
				divs_converge.push_back(i);
		}
		//int num_decrease;
		//for (size_t i = 0; i < m_num_divs; i++)	{
		//	num_decrease = 0;
		//	for (size_t j = 1; j < info.cov_divs[i].size(); ++j) {
		//		if (info.cov_divs[i][j] < info.cov_divs[i][j - 1])
		//			num_decrease++;
		//	}
		//	if (num_decrease == 0)
		//		divs_converge.push_back(i);
		//}
		return divs_converge;
	}

	bool HLC::overVisited(size_t id_bsn_atract, const Solution<> &sol, int id_rnd) const {
		auto &info = *m_info_bsns_atrct[id_bsn_atract];
		auto aff_ssp = getIdxSSP(sol);
		auto prob = info.prob_ssps[info.seq_ssps.at(aff_ssp)];
		return GET_RND(id_rnd).uniform.next() < prob ? false : true;
	}

	void HLC::updateAttractorsFnd(const Solution<> &sol, int id_pro) {
		bool distinct = true;
		for (auto &atrct_fnd : m_attractors_fnd) {
			if (atrct_fnd.variableDistance(sol, id_pro) < GET_CONOP(id_pro).variableNicheRadius()) {
				if (sol.dominate(atrct_fnd, id_pro))
					atrct_fnd = sol;
				distinct = false;
				break;
			}
		}
		if (distinct)
			m_attractors_fnd.emplace_back(sol);
	}

	void HLC::resetSubspaceInfo(size_t id_subspace) {
		m_info_subspaces[id_subspace]->num_imprv = 0;
		//m_info_subspaces[id_subspace]->diff = 1;
		m_info_subspaces[id_subspace]->sols.clear();
		m_info_subspaces[id_subspace]->best_sol.reset();
		m_info_subspaces[id_subspace]->worst_sol.reset();
	}
}
