/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Jun 10, 2019 by Li Zhou

#ifndef OFEC_AMP_CC_CSIWDN_H
#define OFEC_AMP_CC_CSIWDN_H

#include "../../../../../core/algorithm/algorithm.h"
#include "../dyn_sa_multipop.h"
#include "../cc_framework.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif // !OFEC_DEMO

namespace ofec {
	template <typename TPop1, typename TPop2, typename TInd>
	class AMP_CC_CSIWDN : public Algorithm {
		using PopType = CC_framework<TPop1, TPop2, TInd>;
		std::mutex mutex_AMP_out;

	public:
		int sourceIdx();
		const size_t eval_phase() { return m_eval_phase; }
		void run_();
		void initialize_();
		void initMultiPop();

	protected:
		int evolve();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

		int competition();
		void computeNodeDistance(std::vector<Real> & vect);
		void addNewPop(size_t num);
		void output();
		bool isStableByPopNum();

		bool isStableByObj();
		void updateDistance(size_t idx);
		bool isStable();

		std::vector<Real> calCoverageRate();
		void recordIntermediateResult();
		Real calStandardDeviation(const std::vector<float>& vec1, const std::vector<float>& vec2);

		void record() {}

	protected:

		DynSaMultiPop<PopType> m_subpop;
		
		int m_subpop_size;
		int m_num_add;
		std::string m_path_result;
		size_t m_data_update_eval;
		int m_iter = 0;
		bool m_is_first_add = true;
		std::vector<int> m_num_be_removed;
		size_t m_num_curr_pop;

		size_t m_k;
		size_t m_standard_stable;
		size_t m_reserve_stable;
		Real m_max_coverge_rate;

		std::vector<std::vector<Real>> m_coverage_result;
		std::vector<size_t> m_num_pop_result;
		std::vector<Real> m_curr_best_obj;

		size_t m_max_eval;
		size_t m_initial_phase;
		size_t m_final_phase;
		size_t m_eval_phase = 72;  //延迟的优化时间12h

		size_t m_increment;
	};

	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_subpop_size = std::get<int>(v.at("subpopulation size"));
		m_num_add = std::get<int>(v.at("number of SubPopulations")) / 2;
		m_k = std::get<int>(v.at("number of SubPopulations"));
		m_num_curr_pop = std::get<int>(v.at("number of SubPopulations"));

		m_standard_stable = std::get<int>(v.at("beta"));
		m_reserve_stable = m_standard_stable;
		m_increment = std::get<int>(v.at("alpha"));
		m_max_eval = std::get<int>(v.at("maximum evaluations"));
		m_initial_phase = GET_CSIWDN(m_id_pro).phase();
		m_final_phase = GET_CSIWDN(m_id_pro).totalPhase();
		m_data_update_eval = m_max_eval / (m_final_phase - m_initial_phase + 1);
		m_subpop.clear();
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::initMultiPop() {
		GET_CSIWDN(m_id_pro).get_Q_AHC().initialize(GET_CSIWDN(m_id_pro).getAdjmatrix());
		GET_CSIWDN(m_id_pro).get_Q_AHC().clustering(m_k, GET_CSIWDN(m_id_pro).getAdjmatrix());
		GET_CSIWDN(m_id_pro).setInitType(InitType::AHC);
		for (size_t i = 0; i < m_num_curr_pop; ++i) {  // initialize subpopulation 
			auto subpop = std::make_unique<PopType>(m_subpop_size, m_id_pro);
			GET_CSIWDN(m_id_pro).setPopIdentifier(i);
			subpop->m_first_pop->initialize(m_id_pro, m_id_rnd);
			subpop->m_first_pop->evaluate(m_id_pro, m_id_alg);
			subpop->m_first_pop->updateBest(m_id_pro);

			subpop->m_second_pop->initialize(m_id_pro, m_id_rnd);
			subpop->m_second_pop->evaluate(m_id_pro, m_id_alg);
			subpop->m_second_pop->updateBest(m_id_pro);
			
			subpop->fillEach(m_id_rnd, m_id_pro ,m_id_alg,PopType::FillType::random);
			subpop->updateBest(m_id_pro,m_id_alg);

			m_subpop.append(subpop);
		}
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::run_() {
		initMultiPop();
#ifdef OFEC_DEMO
		updateBuffer();
#endif // OFEC_DEMO
		int tag = kNormalEval;
		size_t num_pop_last_iter = m_num_curr_pop;
		GET_CSIWDN(m_id_pro).setAlgorithmStart();
		while (!terminating() && tag != kTerminate) {
			recordIntermediateResult();
			tag = evolve();
			int num_remove = competition();
			m_num_be_removed.push_back(num_remove);
			m_num_curr_pop -= num_remove;

			if (isStable()) {
				m_num_be_removed.clear();
				if (m_is_first_add) {
					int n = m_num_add + GET_CSIWDN(m_id_pro).get_Q_AHC().cluster_size();
					GET_CSIWDN(m_id_pro).get_Q_AHC().clear();
					GET_CSIWDN(m_id_pro).get_Q_AHC().initialize(GET_CSIWDN(m_id_pro).getAdjmatrix());
					GET_CSIWDN(m_id_pro).get_Q_AHC().clustering(n, GET_CSIWDN(m_id_pro).getAdjmatrix());
					addNewPop(m_num_add);
					m_is_first_add = false;
				}
				else {
					int differ = num_pop_last_iter - m_num_curr_pop;
					if (differ < m_num_add)
						++m_num_add;
					else if (differ > m_num_add)
						--m_num_add;
					int n = m_num_add + GET_CSIWDN(m_id_pro).get_Q_AHC().cluster_size();
					GET_CSIWDN(m_id_pro).get_Q_AHC().clear();
					GET_CSIWDN(m_id_pro).get_Q_AHC().initialize(GET_CSIWDN(m_id_pro).getAdjmatrix());
					GET_CSIWDN(m_id_pro).get_Q_AHC().clustering(n, GET_CSIWDN(m_id_pro).getAdjmatrix());
					addNewPop(m_num_add);
				}
				num_pop_last_iter = m_num_curr_pop;
			}

			if (m_effective_eval >= (GET_CSIWDN(m_id_pro).phase() - m_initial_phase + 1) * m_data_update_eval) {
				GET_CSIWDN(m_id_pro).phaseNext();
				if (GET_CSIWDN(m_id_pro).phase() % 6 == 0 && GET_CSIWDN(m_id_pro).phase() < GET_CSIWDN(m_id_pro).totalPhase())
					m_standard_stable = m_reserve_stable;
				if (GET_CSIWDN(m_id_pro).isTimeOut()) break;
				if (GET_CSIWDN(m_id_pro).phase() % m_increment == 0)
					m_standard_stable += 1;
			}

#ifdef OFEC_DEMO
			updateBuffer();
#endif // OFEC_DEMO
		}
		output();
	}

	template <typename TPop1, typename TPop2, typename TInd>
	int AMP_CC_CSIWDN<TPop1, TPop2, TInd>::sourceIdx() {
		int z = 0;
		while ((z + 1) < GET_CSIWDN(m_id_pro).numSource() && GET_CSIWDN(m_id_pro).phase() >= ((*(m_subpop[0].m_first_pop))[0].variable().startTime(z + 1) / GET_CSIWDN(m_id_pro).intervalTimeStep())) {
			z++;
		}
		return z;
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::recordIntermediateResult() {
		m_coverage_result.push_back(calCoverageRate());
		m_num_pop_result.push_back(m_subpop.size());
		Real best = m_subpop[0].m_best->objective()[0];
		for (size_t i = 1; i < m_subpop.size(); ++i) {
			if (best > m_subpop[i].m_best->objective()[0])
				best = m_subpop[i].m_best->objective()[0];
		}
		m_curr_best_obj.push_back(best);
	}

	template <typename TPop1, typename TPop2, typename TInd>
	std::vector<Real> AMP_CC_CSIWDN<TPop1, TPop2, TInd>::calCoverageRate() {
		std::vector<bool> node_coverage(GET_CSIWDN(m_id_pro).numberNode(), false);
		int number_source = GET_CSIWDN(m_id_pro).numberSource();
		std::vector <std::vector<bool>> coverage(number_source, node_coverage);
		std::vector<int> count(number_source, 0);
		int z = sourceIdx();
		for (size_t i = 0; i < m_subpop.size(); ++i) {
			for (size_t j = 0; j < m_subpop[i].m_first_pop->size(); ++j) {
				for (size_t k = 0; k <= z; k++) {
					int index = (*(m_subpop[i].m_first_pop))[j].variable().index(k) - 1;
					if (!coverage[k][index]) { coverage[k][index] = true; ++count[k]; }
				}
			}
		}
		std::vector<Real> count_coverage(number_source, 0);
		for (size_t i = 0; i < number_source; i++) {
			count_coverage[i] = (Real)count[i] / (Real)GET_CSIWDN(m_id_pro).numberNode();
		}
		return count_coverage;
	}

	template <typename TPop1, typename TPop2, typename TInd>
	Real AMP_CC_CSIWDN<TPop1, TPop2, TInd>::calStandardDeviation(const std::vector<float>& vec1, const std::vector<float>& vec2) {
		if (vec1.size() != vec2.size()) return -1;
		int size = vec1.size();
		Real sum = 0;
		for (size_t i = 0; i < size; ++i) {
			sum += pow(vec1[i] - vec2[i], 2);
		}
		return sqrt(sum / (Real)size);
	}

	template <typename TPop1, typename TPop2, typename TInd>
	int AMP_CC_CSIWDN<TPop1, TPop2, TInd>::evolve() {
		int tag = kNormalEval;
		m_subpop.evolve(m_id_pro, m_id_alg, m_id_rnd);
		++m_iter;
		return tag;
	}

#ifdef OFEC_DEMO
	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::updateBuffer() {
		m_solution.clear();
		m_solution.resize(m_subpop.size());
		for (size_t k = 0; k < m_subpop.size(); k++) {
			for (size_t i = 0; i < m_subpop[k].m_first_pop->size(); ++i)
				m_solution[k].push_back(&m_subpop[k].m_first_pop->at(i));
		}
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif // OFEC_DEMO

	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::updateDistance(size_t idx) {
		Real min_distance;
		int z = sourceIdx();
		for (size_t j = 0; j < m_subpop[idx].m_first_pop->size(); ++j) {
			min_distance = std::numeric_limits<Real>::max();
			for (size_t k = 0; k < m_subpop.size(); ++k) {
				if (idx == k) continue;
				Real dis = GET_CSIWDN(m_id_pro).calculateDistance((*(m_subpop[idx].m_first_pop))[j].variable().index(z), m_subpop[k].m_best->variable().index(z));

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_subpop[idx].m_first_pop))[j].m_distance_fitness = min_distance;
		}
		for (size_t j = 0; j < m_subpop[idx].m_second_pop->size(); ++j) {
			min_distance = std::numeric_limits<Real>::max();
			for (size_t k = 0; k < m_subpop.size(); ++k) {
				if (idx == k) continue;
				Real dis = GET_CSIWDN(m_id_pro).calculateDistance((*(m_subpop[idx].m_second_pop))[j].variable().index(z), m_subpop[k].m_best->variable().index(z));

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_subpop[idx].m_second_pop))[j].m_distance_fitness = min_distance;
		}

		for (size_t j = 0; j < m_subpop[idx].m_first_pop->size(); ++j) {
			min_distance = std::numeric_limits<Real>::max();
			for (size_t k = 0; k < m_subpop.size(); ++k) {
				if (idx == k) continue;
				Real dis = GET_CSIWDN(m_id_pro).calculateDistance((*(m_subpop[idx].m_first_pop))[j].trial().variable().index(z), m_subpop[k].m_best->variable().index(z));

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_subpop[idx].m_first_pop))[j].m_pu_distance_fitness = min_distance;
		}
		for (size_t j = 0; j < m_subpop[idx].m_second_pop->size(); ++j) {
			min_distance = std::numeric_limits<Real>::max();
			for (size_t k = 0; k < m_subpop.size(); ++k) {
				if (idx == k) continue;
				Real dis = GET_CSIWDN(m_id_pro).calculateDistance((*(m_subpop[idx].m_second_pop))[j].trial().variable().index(z), m_subpop[k].m_best->variable().index(z));

				if (min_distance > dis) min_distance = dis;
			}
			(*(m_subpop[idx].m_second_pop))[j].m_pu_distance_fitness = min_distance;
		}
	}

	template <typename TPop1, typename TPop2, typename TInd>
	bool AMP_CC_CSIWDN<TPop1, TPop2, TInd>::isStableByPopNum() {
		if (m_num_be_removed.size() < m_standard_stable) return false;
		int i = m_num_be_removed.size() - 1;
		int count = 0;
		while (m_num_be_removed[i] == 0 && m_num_be_removed[i - 1] == 0) {
			++count;
			--i;
			if (i == 0) break;
		}
		return count >= m_standard_stable - 1;
	}

	template <typename TPop1, typename TPop2, typename TInd>
	bool AMP_CC_CSIWDN<TPop1, TPop2, TInd>::isStableByObj() {
		for (size_t i = 0; i < m_subpop.size(); ++i) {
			if (!m_subpop[i].isStableObj()) {
				return false;
			}
		}
		return true;
	}

	template <typename TPop1, typename TPop2, typename TInd>
	bool AMP_CC_CSIWDN<TPop1, TPop2, TInd>::isStable() {

		int i = m_coverage_result.size() - 1;
		if (m_coverage_result.size() < m_standard_stable) return false;
		for (int j = 0; j < GET_CSIWDN(m_id_pro).numberSource(); j++) {
			if (m_coverage_result[i][j] == 1) return false;
		}
		int count = 0;
		while (m_coverage_result[i] == m_coverage_result[i - 1]) {
			++count;
			--i;
			if (i == 0) break;
		}
		return count >= m_standard_stable - 1;
	}


	template <typename TPop1, typename TPop2, typename TInd>
	int AMP_CC_CSIWDN<TPop1, TPop2, TInd>::competition() {
		std::vector<size_t> subpop_indx;
		int z = sourceIdx();
		for (size_t i = 0; i < m_subpop.size(); ++i) {
			bool flag = false;
			for (size_t j = 0; j < subpop_indx.size(); ++j) {
				if (m_subpop[subpop_indx[j]].m_best->variable().index(z) == m_subpop[i].m_best->variable().index(z)) {
					m_subpop.populationCompetition(subpop_indx[j], i);
					if (m_subpop[i].m_best->objective()[0] < m_subpop[subpop_indx[j]].m_best->objective()[0])
						*(m_subpop[subpop_indx[j]].m_best) = *(m_subpop[i].m_best);

					flag = true;
					break;
				}
			}
			if (!flag)
				subpop_indx.push_back(i);
		}
		int differ1 = m_subpop.size();
		int differ2 = subpop_indx.size();
		int differ = differ1 - differ2;
		int return_value = differ;
		if (differ > 0) {

			DynSaMultiPop<PopType> temp(subpop_indx.size(), m_subpop[0].m_first_pop->size(),m_id_pro);
			for (size_t i = 0; i < subpop_indx.size(); ++i) {
				temp[i] = m_subpop[subpop_indx[i]];
			}
			for (size_t i = 0; i < subpop_indx.size(); ++i) {
				m_subpop[i] = temp[i];
			}
		}
		while (differ) {
			m_subpop.popBack();
			--differ;
		}
		return return_value;
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::addNewPop(size_t num) {
		computeNodeDistance(GET_CSIWDN(m_id_pro).distanceNode());
		m_subpop.addNewPopulation(m_id_alg, m_id_pro,m_id_rnd, num, InitType::BeVisited);
		m_num_curr_pop += num;
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::computeNodeDistance(std::vector<Real> & vect) {
		size_t node_size = GET_CSIWDN(m_id_pro).numberNode();
		int z = sourceIdx();
		vect.resize(node_size);
		for (size_t i = 0; i < node_size; ++i) {
			Real curr_best;
			for (size_t j = 0; j < m_subpop.size(); ++j) {
				Real value = GET_CSIWDN(m_id_pro).calculateDistance(i + 1, m_subpop[j].m_best->variable().index(z));
				if (j == 0) curr_best = value;
				else if (curr_best > value) curr_best = value;
			}
			vect[i] = curr_best;
		}
	}

	template <typename TPop1, typename TPop2, typename TInd>
	void AMP_CC_CSIWDN<TPop1, TPop2, TInd>::output() {
	/*	mutex_AMP_out.lock();
		std::stringstream path;
		path << g_working_dir << "result/new/Alg4Epa/AMP/" << m_path_result;
		std::ofstream fout(path.str(), std::ios::app);
		fout << std::endl;
		fout << "runID " << m_id_alg << ": " << std::endl;

		float j = 0;
		float best = m_subpop[0].m_best->objective()[0];
		for (size_t i = 0; i < m_subpop.size(); ++i) {
			if (best > m_subpop[i].m_best->objective()[0]) {
				best = m_subpop[i].m_best->objective()[0];
				j = i;
			}
		}
		fout << "global optimum:" << std::endl;
		fout << "population index:" << j + 1 << std::endl;
		fout << "obj: " << best << std::endl;
		for (size_t z = 0; z < GET_CSIWDN(m_id_pro).numberSource(); z++) {
			fout << "location: " << m_subpop[j].m_best->variable().location(z) << std::endl;
			fout << "start time: " << m_subpop[j].m_best->variable().startTime(z) << std::endl;
			fout << "duration: " << m_subpop[j].m_best->variable().duration(z) << std::endl;
			fout << "multiplier: " << std::endl;
			for (auto& mul_value : m_subpop[j].m_best->variable().multiplier(z))
				fout << mul_value << "  ";
			fout << std::endl;
			fout << "standard deviation: " << calStandardDeviation(m_subpop[j].m_best->variable().multiplier(z), GET_CSIWDN(m_id_pro).getOptima().variable(0).multiplier(z)) << std::endl;
			fout << std::endl;
		}

		fout << std::endl;
		for (size_t i = 0; i < m_subpop.size(); ++i) {
			fout << "best solution " << i + 1 << ": " << std::endl;
			size_t z;
			for (z = 0; z < GET_CSIWDN(m_id_pro).numberSource(); z++) {
				fout << "location: " << m_subpop[i].m_best->variable().location(z) << std::endl;
				fout << "start time: " << m_subpop[i].m_best->variable().startTime(z) << std::endl;
				fout << "duration: " << m_subpop[i].m_best->variable().duration(z) << std::endl;
				fout << "multiplier: " << std::endl;
				for (auto& mul_value : m_subpop[i].m_best->variable().multiplier(z))
					fout << mul_value << "  ";
				fout << std::endl;
				fout << "standard deviation: " << calStandardDeviation(m_subpop[i].m_best->variable().multiplier(z), GET_CSIWDN(m_id_pro).getOptima().variable(0).multiplier(z)) << std::endl;
				fout << std::endl;
			}
			fout << "obj: " << m_subpop[i].m_best->objective()[0] << std::endl;
			fout << std::endl;
			fout << "concentration: " << std::endl;
			std::vector<std::vector<float>> concen_data(GET_CSIWDN(m_id_pro).numSensor(), std::vector<float>(144));
			GET_CSIWDN(m_id_pro).getData(m_subpop[i].m_best->variable(), concen_data);
			for (size_t j = 0; j < 144; ++j) {
				fout << concen_data[0][j] << " " << concen_data[1][j] << " " << concen_data[2][j] << " " << concen_data[3][j] << " " << (concen_data[0][j] + concen_data[1][j] + concen_data[2][j] + concen_data[3][j]) / 4.0 << std::endl;
			}
		}

		for (size_t i = 0; i < m_coverage_result.size(); ++i)
			fout << i << "   " << m_num_pop_result[i] << "   " << m_coverage_result[i] << std::endl;

		fout << "----------------------------------------------------" << std::endl;

		fout.close();

		std::stringstream path2;
		path2 << g_working_dir << "result/new/Alg4Epa/AMP/NET2_97/20w/curr_best_obj_0715.txt";
		std::ofstream fout2(path2.str(), std::ios::app);
		for (size_t i = 0; i < m_curr_best_obj.size(); ++i)
			fout2 << i << "   " << m_curr_best_obj[i] << std::endl;
		fout2.close();

		mutex_AMP_out.unlock();*/
	}

}

#endif //! OFEC_AMP_CC_CSIWDN_H