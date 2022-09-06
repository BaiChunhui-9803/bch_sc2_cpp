#include "sade_population.h"
#include "../../../problem/realworld/csiwdn/csiwdn.h"

namespace ofec {
	SaDEPopulation::SaDEPopulation(size_t no, int id_pro, size_t dim) : 
		Population(no,id_pro,dim), 
		m_F(no, std::vector<Real>(GET_CSIWDN(id_pro).numberSource())),
		m_cnt_success(GET_CSIWDN(id_pro).numberSource()),
		m_cnt_fail(GET_CSIWDN(id_pro).numberSource()),
		m_CRsuc(GET_CSIWDN(id_pro).numberSource()),
		mvv_CR(no, std::vector<std::vector<Real>>(GET_CSIWDN(id_pro).numberSource(), std::vector<Real>(m_num_strategy))),
		m_CRm(GET_CSIWDN(id_pro).numberSource(),std::vector<Real>(m_num_strategy, 0.5)), 
		m_pro_strategy(GET_CSIWDN(id_pro).numberSource(),std::vector<Real>(m_num_strategy, 1. / m_num_strategy)),
		m_strategy_selection(no,std::vector<int>(GET_CSIWDN(id_pro).numberSource())), m_probability(2), 
		m_ST_data_obj(GET_CSIWDN(id_pro).maxStartTimeSize(), std::make_pair(0.0, 0)),
		m_duration_data_obj(GET_CSIWDN(id_pro).maxDurationSize(), std::make_pair(0.0, 0)) \
	{
		m_probability[0].resize(GET_CSIWDN(id_pro).maxStartTimeSize());
		m_probability[1].resize(GET_CSIWDN(id_pro).maxDurationSize());

		for (size_t j = 0; j < GET_CSIWDN(id_pro).numberSource(); ++j) {
			for (auto i = 0; i < m_pro_strategy.size(); ++i) {
				if (i > 0) m_pro_strategy[j][i] += m_pro_strategy[j][i - 1];
			}
		}
	}

	void SaDEPopulation::updateF(int id_pro,int id_rnd) {
		size_t z = 0, q = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((m_inds[0]->variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
			z++;
		}
		//int n = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((q + 1) < GET_CSIWDN(id_pro).numberSource() && (n * GET_CSIWDN(id_pro).evalPhase()) >= ((m_inds[0]->variable().startTime(q + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	q++;
		//}
		for (auto i = 0; i < size(); i++) {
			for (int j = q; j <= z; ++j) {
				m_F[i][j] = GET_RND(id_rnd).uniform.nextNonStd(0.5, 0.09);
			}
		}
	}

	void SaDEPopulation::updateCR(int id_pro, int id_rnd) {
		size_t z = 0, q = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((m_inds[0]->variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
			z++;
		}
		//int n = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((q + 1) < GET_CSIWDN(id_pro).numberSource() && (n * GET_CSIWDN(id_pro).evalPhase()) >= ((m_inds[0]->variable().startTime(q + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	q++;
		//}
		if (m_iter >= m_LP) {
			for (int j = q; j <= z; ++j) {
				for (int k = 0; k < m_num_strategy; ++k) {
					std::vector<std::vector<Real>> t;
					for (auto it = m_CRsuc[j].begin(); it != m_CRsuc[j].end(); ++it) {
						for (auto i : it->at(k)) t[j].push_back(i);
					}
					if (t.size() > 0) {
						nth_element(t[j].begin(), t[j].begin() + t[j].size() / 2, t[j].end()); //中位数放中间
						m_CRm[j][k] = t[j][(t[j].size()) / 2];  //选中位数
					}
				}
			}
		}
		for (auto i = 0; i < size(); i++) {
			for (int j = q; j <= z; ++j) {
				for (int k = 0; k < m_num_strategy; ++k) {
					do {
						mvv_CR[i][j][k] = GET_RND(id_rnd).uniform.nextNonStd(m_CRm[j][k], 0.01);
					} while (mvv_CR[i][j][k] < 0 || mvv_CR[i][j][k]>1);
				}
			}
		}

	}

	void SaDEPopulation::updateProStrategy(int id_pro) {

		std::vector<std::vector < std::list<Real>>> curmem(GET_CSIWDN(id_pro).numberSource(), std::vector<std::list<Real>>(m_num_strategy));
		std::vector<std::vector<int>> curSuc(GET_CSIWDN(id_pro).numberSource(), std::vector <int>(m_num_strategy)), curFail(GET_CSIWDN(id_pro).numberSource(), std::vector <int>(m_num_strategy));
		size_t z = 0, q = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((m_inds[0]->variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
			z++;
		}
		//int n = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((q + 1) < GET_CSIWDN(id_pro).numberSource() && (n * GET_CSIWDN(id_pro).evalPhase()) >= ((m_inds[0]->variable().startTime(q + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	q++;
		//}
		for (auto i = 0; i < size(); i++) {
			for (int j = q; j <= z; ++j) {
				if (m_inds[i]->isImproved()) {
					curmem[j][m_strategy_selection[i][j]].push_back(mvv_CR[i][j][m_strategy_selection[i][j]]);
					curSuc[j][m_strategy_selection[i][j]]++;
				}
				else {
					curFail[j][m_strategy_selection[i][j]]++;
				}
			}
		}

		for (int j = q; j <= z; ++j) {
			m_cnt_success[j].push_back(move(curSuc[j]));
			m_CRsuc[j].push_back(move(curmem[j]));
			m_cnt_fail[j].push_back(move(curFail[j]));

			if (m_iter >= m_LP) {
				m_cnt_success[j].pop_front();
				m_CRsuc[j].pop_front();
				m_cnt_fail[j].pop_front();

				//update probability for all stategies
				for (int k = 0; k < m_num_strategy; ++k) {
					m_pro_strategy[j][k] = 0;
					std::vector<int> fail(GET_CSIWDN(id_pro).numberSource(), 0);
					for (auto& f : m_cnt_success[j]) m_pro_strategy[j][k] += f[k];
					for (auto& f : m_cnt_fail[j]) fail[j] += f[k];

					m_pro_strategy[j][k] = m_pro_strategy[j][k] / (m_pro_strategy[j][k] + fail[j]) + m_epsilon;
					if (k > 0) m_pro_strategy[j][k] += m_pro_strategy[j][k - 1];
				}
			}
		}
	}

	void SaDEPopulation::mutate(int id_pro,int id_rnd) {
		size_t z = 0, q = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((m_inds[0]->variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
			z++;
		}
		//int n = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((q + 1) < GET_CSIWDN(id_pro).numberSource() && (n * GET_CSIWDN(id_pro).evalPhase()) >= ((m_inds[0]->variable().startTime(q + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	q++;
		//}
		for (size_t i = 0; i < size(); ++i) {
			for (int j = q; j <= z; ++j) {
				Real p = GET_RND(id_rnd).uniform.next() * m_pro_strategy[j][m_num_strategy - 1];
				m_strategy_selection[i][j] = lower_bound(m_pro_strategy[j].begin(), m_pro_strategy[j].end(), p) - m_pro_strategy[j].begin();
				setMutationStrategy(DEMutationSratgy(m_strategy_selection[i][j]));
				updateBest(id_pro);
				mutate_(id_pro, id_rnd, i,j, m_F[i][j], m_probability);
			}
		}
	}

	void SaDEPopulation::setMutationStrategy(DEMutationSratgy rS) {
		m_mutation_strategy = rS;
	}

	void SaDEPopulation::recombine(int id_pro, int id_rnd) {
		size_t z = 0, q = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((m_inds[0]->variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
			z++;
		}
		//int n = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((q + 1) < GET_CSIWDN(id_pro).numberSource() && (n * GET_CSIWDN(id_pro).evalPhase()) >= ((m_inds[0]->variable().startTime(q + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	q++;
		//}
		for (size_t i = 0; i < size(); ++i) {
			for (int j = q; j <= z; ++j) {
				m_inds[i]->recombine(id_pro, id_rnd, mvv_CR[i][j][m_strategy_selection[i][j]]);
			}
		}
	}
	
	void SaDEPopulation::updateProbability(int id_pro) {
		
		/// update probability of starting time
		int z = 0;
		for (auto &i : m_inds) {
			while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((i->variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
				z++;
			}
			int idx = (i->variable().startTime(z) - GET_CSIWDN(id_pro).minStartTime()) / GET_CSIWDN(id_pro).patternStep();
			m_ST_data_obj[idx].first += i->objective()[0];
			++(m_ST_data_obj[idx].second);
		}
		std::vector<Real> mean_ST(m_ST_data_obj.size(), 0.0);
		size_t count_ST = 0;
		for (size_t i = 0; i < GET_CSIWDN(id_pro).maxStartTimeSize(); ++i) {
			if (m_ST_data_obj[i].second != 0) {
				mean_ST[i] = m_ST_data_obj[i].first / m_ST_data_obj[i].second;
				++count_ST;
			}
		}

		Real max = 0;
		for (auto &i : mean_ST)
			if (max < i) max = i;
		std::vector<Real> inverse;
		int size_ST = mean_ST.size();
		for (auto &i : mean_ST) {
			if (i == 0 || i == max)
				inverse.push_back(max / size_ST);
			else
				inverse.push_back(max - i);
		}
		Real sum = 0;
		for (auto &i : inverse)
			sum += i;
		m_probability[0].clear();
		for (auto &i : inverse)
			m_probability[0].push_back(i / sum);

		/// update probability of duration
		z = 0;
		for (auto &i : m_inds) {
			while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((i->variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
				z++;
			}
			int idx = (i->variable().duration(z) - GET_CSIWDN(id_pro).minDuration()) / GET_CSIWDN(id_pro).patternStep();
			m_duration_data_obj[idx].first += i->objective()[0];
			++(m_duration_data_obj[idx].second);
		}
		std::vector<Real> mean_duration(m_duration_data_obj.size(), 0.0);
		size_t count_duration = 0;
		for (size_t i = 0; i < GET_CSIWDN(id_pro).maxDurationSize(); ++i) {
			if (m_duration_data_obj[i].second != 0) {
				mean_duration[i] = m_duration_data_obj[i].first / m_duration_data_obj[i].second;
				++count_duration;
			}
		}

	    max = 0;
		for (auto &i : mean_duration)
			if (max < i) max = i;
		inverse.clear();
		int size_duration = mean_duration.size();
		for (auto &i : mean_duration) {
			if (i == 0 || i == max)
				inverse.push_back(max / size_duration);
			else
				inverse.push_back(max - i);
		}
		sum = 0;
		for (auto &i : inverse)
			sum += i;
		m_probability[1].clear();
		for (auto &i : inverse)
			m_probability[1].push_back(i / sum);
	}

	void SaDEPopulation::mutate_(int id_pro, int id_rnd, const int idx, const int jdx, Real F, const std::vector<std::vector<Real>> & pro) {
		std::vector<int> ridx;
		switch (m_mutation_strategy) {
		case rand_1:
			select(id_rnd,idx, 3, ridx);
			this->m_inds[idx]->mutateSecondPart(id_pro, F, jdx, pro, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		case best_1:
			select(id_rnd, idx, 2, ridx);
			this->m_inds[idx]->mutateSecondPart(id_pro, F, jdx, pro, m_best.front().get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
			break;
		case target_to_best_1:
			select(id_rnd, idx, 2, ridx);
			this->m_inds[idx]->mutateSecondPart(id_pro, F, jdx, pro, m_inds[idx].get(), m_best.front().get(), m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get());
			break;
		case best_2:
			select(id_rnd, idx, 4, ridx);
			this->m_inds[idx]->mutateSecondPart(id_pro, F, jdx, pro, m_best.front().get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get());
			break;
		case rand_2:
			select(id_rnd, idx, 5, ridx);
			this->m_inds[idx]->mutateSecondPart(id_pro, F, jdx, pro, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get(), m_inds[ridx[3]].get(), m_inds[ridx[4]].get());
			break;
		case rand_to_best_1:
			select(id_rnd, idx, 3, ridx);
			this->m_inds[idx]->mutateSecondPart(id_pro, F, jdx, pro, m_inds[ridx[0]].get(), m_best.front().get(), m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		case target_to_rand_1:
			select(id_rnd,idx, 3, ridx);
			this->m_inds[idx]->mutateSecondPart(id_pro,F, jdx, pro, m_inds[idx].get(), m_inds[ridx[0]].get(), m_inds[idx].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			break;
		}
	}

	void SaDEPopulation::select(int id_rnd,int base, int number, std::vector<int>& result) {
		std::vector<int> candidate;
		for (int i = 0; i < this->m_inds.size(); ++i) {
			if (base != i) candidate.push_back(i);
		}
		result.resize(number);
		for (int i = 0; i < number; ++i) {
			int idx = GET_RND(id_rnd).uniform.nextNonStd<int>(0, (int)candidate.size() - i);
			result[i] = candidate[idx];
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}

	void SaDEPopulation::fillSolution(VarCSIWDN& indi,int id_pro) {
		for (auto &i : m_inds)
			i->coverFirstPart(indi, id_pro);
	}

	void SaDEPopulation::select(int id_pro, int id_alg, bool is_stable) {
		for (auto &i : m_inds)
			i->select(id_pro, id_alg,is_stable);

	}

	bool SaDEPopulation::isFeasiblePopulation(int id_pro,const Real tar) {

		size_t phase = GET_CSIWDN(id_pro).phase();
		size_t interval = GET_CSIWDN(id_pro).interval();
		size_t num = phase*interval;
		Real temp = 0;

		for (size_t i = 0; i < num; ++i) {
			for (int j = 0; j < GET_CSIWDN(id_pro).numSensor(); ++j) {
				temp += pow(GET_CSIWDN(id_pro).observationConcentration()[j][i], 2);
			}
		}

		Real benchmark = tar * sqrt(temp / (GET_CSIWDN(id_pro).numSensor()*num));

		size_t count_feasible = 0, count_infeasible = 0;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			if (m_inds[i]->objective()[0] <= benchmark) ++count_feasible;
			else ++count_infeasible;
		}

		return count_feasible >= count_infeasible;
	}
}