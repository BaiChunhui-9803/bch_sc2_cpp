#include "rcr_vec_real_dynamic.h"
#include "../../../core/instance_manager.h"
#include "../../../core/problem/continuous/continuous.h"
#include <fstream>

namespace ofec {
	void RecordVecRealDynamic::outputData(const std::list<int>& ids_pros, const std::list<int>& ids_algs) {
		int id_pro = ids_pros.front();
		if (GET_PRO(id_pro).hasTag(ProTag::kConOP)) {
			auto& optima = GET_CONOP(id_pro).getOptima();
			//m_heading.push_back("Evaluations");
			//m_heading.push_back("Number of environment");
			//m_heading.push_back("Optimal objective value");
			//m_heading.push_back("Best objective value so far");

			m_heading.push_back("Offline Error");
			m_heading.push_back("Best Before Change Error");

		}
		outputProgress();
		outputFinal();
	}

	void RecordVecRealDynamic::record(int id_alg, const std::vector<Real>& vals) {
		m_data[id_alg].insert(m_data[id_alg].end(), vals.begin(), vals.end());
	}

	void RecordVecRealDynamic::outputProgress() {
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

	//void RecordVecRealDynamic::outputFinal() {
	//	size_t size_row = m_heading.size();
	//	// intercept the final record of each run
	//	std::vector<std::vector<Real>> temp_data;
	//	std::vector<std::vector<Real>> final_data;
	//	for (auto& rcr_run : m_data) {
	//		temp_data.clear();
	//		for (size_t i = 0; i < rcr_run.second.size() / m_heading.size(); i++) {
	//			Real arr[4] = { *(rcr_run.second.begin() + m_heading.size() * i),*(rcr_run.second.begin() + m_heading.size() * i + 1),
	//			*(rcr_run.second.begin() + m_heading.size() * i + 2),*(rcr_run.second.begin() + m_heading.size() * i + 3) };
	//			std::vector<Real> data(arr, arr + m_heading.size());
	//			temp_data.push_back(data);
	//		}
	//		Real mean_error(0.);
	//		for (auto& i : temp_data) {
	//			mean_error += abs(*(i.begin() + size_row - 2) - *(i.begin() + size_row - 1));
	//		}
	//		mean_error /= temp_data.size();
	//		final_data.push_back({ mean_error });
	//	}
	//	for (size_t i(0); i < final_data.size(); i++)
	//		std::cout << final_data[i][0] << std::endl;
	//	std::vector<std::pair<Real, Real>> mean_and_var(final_data[0].size(), { 0,0 });
	//	// calculate mean
	//	for (size_t j = 0; j < final_data[0].size(); ++j) {
	//		for (size_t i = 0; i < final_data.size(); i++) {
	//			mean_and_var[j].first += final_data[i][j];
	//		}
	//		mean_and_var[j].first /= final_data.size();
	//	}
	//	// calculate standard error
	//	for (size_t j = 0; j < final_data[0].size(); ++j) {
	//		for (size_t i = 0; i < final_data.size(); i++) {
	//			mean_and_var[j].second += pow((final_data[i][j] - mean_and_var[j].first), 2);
	//		}
	//		mean_and_var[j].second = (sqrt(mean_and_var[j].second / static_cast<Real>(final_data.size() - 1)))/ sqrt(final_data.size());
	//	}
	//	std::ofstream out_file(m_filename.str() + "final.txt");
	//	std::stringstream out;
	//	//output mean and standard error
	//	for (size_t j = 0; j < final_data[0].size(); ++j) {
	//		std::cout << "Mean of " << m_heading[j] << ": " << mean_and_var[j].first << "\n";
	//		std::cout << "Standard error of " << m_heading[j] << ": " << mean_and_var[j].second << "\n";
	//		out << "Mean of " << "Offline Error" << ": " << mean_and_var[j].first << "\n";
	//		out << "Standard error of " << "Offline Error" << ": " << mean_and_var[j].second << "\n";
	//	}
	//	out_file << out.str();
	//	out_file.close();
	//}

	void RecordVecRealDynamic::outputFinal() {
		size_t size_row = m_heading.size();
		// intercept the final record of each run
		std::vector<std::vector<Real>> final_data;
		for (auto& rcr_run : m_data) {
			final_data.emplace_back(rcr_run.second.end() - size_row, rcr_run.second.end());
		}
		//// remove the best & worst result
		//if (final_data.size() > 2) {
		//	int min = 0, max = 0;
		//	for (size_t i(1); i < final_data.size(); i++) {
		//		if (final_data[min][0] < final_data[i][0]) min = i;
		//	}
		//	final_data.erase(final_data.begin() + min);
		//	for (size_t i(1); i < final_data.size(); i++) {
		//		if (final_data[max][0] > final_data[i][0]) max = i;
		//	}
		//	final_data.erase(final_data.begin() + max);
		//}
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
			mean_and_var[j].second = sqrt(mean_and_var[j].second / static_cast<Real>(final_data.size() - 1))/sqrt(final_data.size());
		}
		std::ofstream out_file(m_filename.str() + "final.txt");
		std::stringstream out;
		//output mean and variance
		for (size_t j = 0; j < size_row; ++j) {
			std::cout << "Mean of " << m_heading[j] << ": " << mean_and_var[j].first << "\n";
			std::cout << "Standard error of " << m_heading[j] << ": " << mean_and_var[j].second << "\n";
			out << "Mean of " << m_heading[j] << ": " << mean_and_var[j].first << "\n";
			out << "Standard error of " << m_heading[j] << ": " << mean_and_var[j].second << "\n";
		}
		out_file << out.str();
		out_file.close();
		std::cin.get();
	}
}