#include "F5_schwefel_2_6_bound.h"
#include "../../../../../core/instance_manager.h"
#include "../../../../../core/global.h"
#include <sstream>

namespace ofec {
	namespace cec2005 {
		void Schwefel_2_6_Bound::initialize_() {
			Function::initialize_();
			resizeVariable(std::get<int>(GET_PARAM(m_id_param).at("number of variables")));
			setDomain(-100, 100);
			setOriginalGlobalOpt();
			loadData("/instance/problem/continuous/global/cec2005/data/"); // load data into m_a and m_b
			setBias(-310);
			setGlobalOpt(m_translation.data());
			m_variable_niche_radius = 1e-4 * 100 * m_num_vars;
			m_objective_accuracy = 1.0e-6;
		}

		void Schwefel_2_6_Bound::loadData(const std::string &path) {
			std::string sa;
			std::stringstream ss;
			ss << m_num_vars << "Dim.txt";
			sa = ss.str();
			sa.insert(0, m_name + "_a_");
			sa.insert(0, path);
			sa.insert(0, g_working_dir);// data path

			m_a.assign(m_num_vars, std::vector<Real>(m_num_vars));
			m_b.resize(m_num_vars);

			std::ifstream in_a(sa);
			if (in_a.fail()) {
				for (int i = 0; i < m_num_vars; ++i) {
					for (int j = 0; j < m_num_vars; ++j) {
						m_a[i][j] = int(-500.0 + GET_RND(m_id_rnd).uniform.next() * 1000);
					}
				}
				std::ofstream out(sa.c_str());
				for (int i = 0; i < m_num_vars; ++i) {
					for (int j = 0; j < m_num_vars; j++) {
						out << m_a[i][j] << " ";
					}
				}
				out.close();
			}
			else {
				std::string row;
				for (int i = 0; i < m_num_vars; ++i) {
					std::getline(in_a, row);
					std::stringstream sstr_row(row);
					for (int j = 0; j < m_num_vars; j++) {
						sstr_row >> m_a[i][j];
					}
				}
			}
			in_a.close();

			m_translation.resize(m_num_vars);
			std::string so;
			so = ss.str();
			so.insert(0, m_name + "_Opt_");

			so.insert(0, path);
			so.insert(0, g_working_dir);// data path

			std::ifstream in;
			in.open(so.data());
			if (in.fail()) {
				setTranslation(m_original_optima.variable(0).data());
				for (int i = 0; i < m_num_vars; ++i) {
					if (i < m_num_vars / 4) m_translation[i] = -100;
					else if (i >= m_num_vars * 3 / 4 - 1) m_translation[i] = 100;
				}

				std::ofstream out(so.c_str());
				for (int j = 0; j < m_num_vars; j++)        out << m_translation[j] << " ";
				out.close();
			}
			else {
				for (int j = 0; j < m_num_vars; j++) {
					in >> m_translation[j];
				}
			}
			in.close();

			for (size_t i = 0; i < m_num_vars / 4 + 1; ++i)
				m_translation[i] = -100;
			for (size_t i = m_num_vars * 3 / 4; i < m_num_vars; ++i)
				m_translation[i] = 100;

			for (int i = 0; i < m_num_vars; ++i) {
				m_b[i] = 0;
				for (int j = 0; j < m_num_vars; ++j) {
					m_b[i] += m_a[i][j] * m_translation[j];
				}
			}
		}

		void Schwefel_2_6_Bound::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
			std::vector<Real> temp_vector(m_num_vars);
			for (int i = 0; i < m_num_vars; ++i) {
				for (int j = 0; j < m_num_vars; ++j) {
					temp_vector[j] = 0;
					for (int k = 0; k < m_num_vars; ++k) {
						temp_vector[j] += m_a[j][k] * x[k];
					}
				}
				for (int j = 0; j < m_num_vars; ++j) {
					temp_vector[j] -= m_b[j];
				}
			}
			Real temp_max = abs(temp_vector[0]);
			for (size_t j = 1; j < m_num_vars; ++j) {
				if (abs(temp_vector[j]) > temp_max)
					temp_max = abs(temp_vector[j]);
			}
			obj[0] = temp_max + m_bias;
		}
	}
}