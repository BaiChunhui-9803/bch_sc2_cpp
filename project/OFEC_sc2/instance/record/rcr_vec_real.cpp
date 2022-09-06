#include "rcr_vec_real.h"
#include "../../core/instance_manager.h"
#include "../../core/problem/continuous/continuous.h"
#include <fstream>

namespace ofec {
	void RecordVecReal::outputData(const std::list<int>& ids_pros, const std::list<int>& ids_algs) {
		int id_pro = ids_pros.front();
		if (GET_PRO(id_pro).hasTag(ProTag::kConOP)) {
			auto& optima = GET_CONOP(id_pro).getOptima();
			m_heading.push_back("Evaluations");
			if (GET_PRO(id_pro).hasTag(ProTag::kDOP)) {
				m_heading.push_back("Number of environment");
				m_heading.push_back("Optimal objective value");
				m_heading.push_back("Best objective value so far");
			}
			else if (GET_PRO(id_pro).hasTag(ProTag::kMMOP)) {
				m_heading.push_back("Number of optima found");
				m_heading.push_back("Success Rate");
			}
			else {
				m_heading.push_back("Error");
			}
		}
		/*outputProgress();*/
		outputFinal();
	}
	
	void RecordVecReal::record(int id_alg, const std::vector<Real>& vals) {
		m_data[id_alg].insert(m_data[id_alg].end(), vals.begin(), vals.end());
	}

	void RecordVecReal::outputProgress() {
		std::vector<Real> sum;
		size_t max_len = 0;
		size_t size_row = m_heading.size();
		for (auto& row : m_data)
			if (max_len < row.second.size())
				max_len = row.second.size();
		for (auto& rcr_run : m_data) {
			if (rcr_run.second.size() < max_len) {
				size_t size = rcr_run.second.size();
				std::vector<Real> temp_row;
				for (size_t j = size_row; j > 0; --j)
					temp_row.push_back(rcr_run.second[size - j]);
				for (size_t j = size; j < max_len; j += size_row)
					for (auto value : temp_row)
						rcr_run.second.push_back(value);
			}
		}
		sum.resize(max_len, 0);
		for (size_t i = 0; i < max_len; i += size_row) {
			for (auto& rcr_run : m_data)
				for (size_t k = i; k < i + size_row; ++k)
					sum[k] += rcr_run.second[k];
			for (size_t k = i; k < i + size_row; ++k)
				sum[k] /= m_data.size();
		}
		std::ofstream out_file(m_filename.str() + "progr.csv");
		std::stringstream out;
		// output heading
		for (size_t i = 0; i < size_row - 1; i++)
			out << m_heading[i] << ",";
		out << m_heading[size_row - 1] << "\n";
		// output data
		for (size_t i = 0; i < max_len - size_row + 1; i += size_row) {
			for (size_t j = i; j < i + size_row - 1; ++j)
				out << sum[j] << ",";
			out << sum[i + size_row - 1] << "\n";
		}
		out_file << out.str();
		out_file.close();
	}

	void RecordVecReal::outputFinal() {
		size_t size_row = m_heading.size();
		// intercept the final record of each run
		std::vector<std::vector<Real>> final_data;
		for (auto& rcr_run : m_data) {
			final_data.emplace_back(rcr_run.second.end() - size_row, rcr_run.second.end());
		}
		std::vector<std::pair<Real, Real>> mean_and_var(size_row, { 0,0 });
		// calculate mean
		for (size_t j = 0; j < size_row; ++j) {
			for (size_t i = 0; i < final_data.size(); i++) {
				mean_and_var[j].first += final_data[i][j];
			}
			mean_and_var[j].first /= final_data.size();
		}
		// calculate variance
		for (size_t j = 0; j < size_row; ++j) {
			for (size_t i = 0; i < final_data.size(); i++) {
				mean_and_var[j].second += pow((final_data[i][j] - mean_and_var[j].first), 2);
			}
			mean_and_var[j].second = sqrt(mean_and_var[j].second / static_cast<Real>(final_data.size() - 1));
		}
		std::ofstream out_file(m_filename.str() + "final.txt");
		std::stringstream out;
		//output mean and variance
		for (size_t j = 0; j < size_row; ++j) {
			out << "Mean of " << m_heading[j] << ": " << mean_and_var[j].first << "\n";
			out << "St.D. of " << m_heading[j] << ": " << mean_and_var[j].second << "\n";
		}
		out_file << out.str();
		out_file.close();
	}
}