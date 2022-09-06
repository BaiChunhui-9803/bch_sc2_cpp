#include "gl_population.h"
#include "../../../problem/realworld/csiwdn/csiwdn.h"

namespace ofec {

	 GLPopulation:: GLPopulation(size_t no, int id_pro, size_t dim) : 
		 Population(no, id_pro, dim), 
		 m_probability(GET_CSIWDN(id_pro).numberNode(), std::vector<Real>(GET_CSIWDN(id_pro).numberNode())),
		 m_node_data_obj(GET_CSIWDN(id_pro).numberNode(), std::vector<std::pair<Real, size_t>>(GET_CSIWDN(id_pro).numberNode(), std::make_pair(0.0, 0))),
		 source_index(0) {}

	void  GLPopulation::mutate(int id_pro,int id_rnd) {
		for (size_t i = 0; i < size(); ++i) {
			m_inds[i]->mutateFirstPart(m_probability,id_pro,id_rnd);
		}
	}

	void  GLPopulation::updateProbability(int id_pro) {
		/// update probability of node
		int z = 0, q = 0;
		for (auto &i : m_inds) {
			while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= (i->variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep())) {
				z++;
			}
			//int n = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
			//while ((q + 1) < GET_CSIWDN(id_pro).numberSource() && (n * GET_CSIWDN(id_pro).evalPhase()) >= (i->variable().startTime(q + 1) / GET_CSIWDN(id_pro).intervalTimeStep())) {
			//	q++;
			//}		
			if (GET_CSIWDN(id_pro).indexFlag(i->variable().index(q) - 1)) {
				for (int j = q; j <= z; j++) {
					m_node_data_obj[j][i->variable().index(j) - 1].first += i->objective()[0];
					++(m_node_data_obj[j][i->variable().index(j) - 1].second);
				}
			}
		}
		
		std::vector<Real> single_mean_node(m_node_data_obj[0].size(), 0.0);
		std::vector<std::vector<Real>> mean_node(GET_CSIWDN(id_pro).numberSource(), single_mean_node);
		size_t count_node = 0;
		for (size_t i = 0; i <  GET_CSIWDN(id_pro).numberNode(); ++i) {
			for (size_t j = 0; j < GET_CSIWDN(id_pro).numberNode(); ++j) {
				if (m_node_data_obj[i][j].second != 0) {
					mean_node[i][j] = m_node_data_obj[i][j].first / m_node_data_obj[i][j].second;
					++count_node;
				}
			}
		}

		m_probability.clear();
		m_probability.resize(mean_node.size());
		Real max = 0;
		for (int j = q; j <= z; j++) {
			max = 0;
			for (auto& i : mean_node[j])
				if (max < i) max = i;
			std::vector<Real> inverse;
			int size_node = mean_node[j].size();
			for (auto& i : mean_node[j]) {
				if (i == 0 || i == max)
					inverse.push_back(max / size_node);
				else
					inverse.push_back(max - i);
			}

			Real sum = 0;
			for (auto& i : inverse)
				sum += i;

			for (auto& i : inverse)
				m_probability[j].push_back(i / sum);
		}
	}

	void  GLPopulation::fillSolution(VarCSIWDN& indi,int id_pro) {
		for (auto &i : m_inds)
			i->coverSecondPart(indi, id_pro);
	}

	void  GLPopulation::select(int id_pro, int id_alg, bool is_stable) {
		for (auto &i : m_inds)
			i->select(id_pro,id_alg,is_stable);

	}

	bool  GLPopulation::isFeasiblePopulation(int id_pro,const Real tar) {

		size_t phase =  GET_CSIWDN(id_pro).phase();
		size_t interval =  GET_CSIWDN(id_pro).interval();
		size_t num = phase*interval;
		Real temp = 0;

		for (size_t i = 0; i < num; ++i) {
			for (int j = 0; j <  GET_CSIWDN(id_pro).numSensor(); ++j) {
				temp += pow( GET_CSIWDN(id_pro).observationConcentration()[j][i], 2);
			}
		}

		Real benchmark = tar * sqrt(temp / ( GET_CSIWDN(id_pro).numSensor()*num));

		size_t count_feasible = 0, count_infeasible = 0;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			if (m_inds[i]->objective()[0] <= benchmark) ++count_feasible;
			else ++count_infeasible;
		}

		return count_feasible >= count_infeasible;


	}
}