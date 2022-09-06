#include "F12_schwefel_2_13.h"
#include "../../../../../core/instance_manager.h"
#include "../../../../../core/global.h"
#include <sstream>

namespace ofec {
	namespace cec2005 {
		void Schwefel_2_13::initialize_() {
			Function::initialize_();
			resizeVariable(std::get<int>(GET_PARAM(m_id_param).at("number of variables")));
			m_a.assign(m_num_vars, std::vector<Real>(m_num_vars));
			m_b.assign(m_num_vars, std::vector<Real>(m_num_vars));
			m_alpha.resize(m_num_vars);		
			setDomain(-OFEC_PI, OFEC_PI);
			loadData("/instance/problem/continuous/global/cec2005/data/");
			setOriginalGlobalOpt();
			setBias(-460);
			m_translation = m_alpha;
			setGlobalOpt(m_translation.data());
			m_variable_niche_radius = 1e-4 * OFEC_PI * m_num_vars;
			m_objective_accuracy = 1.0e-2;
		}

		void Schwefel_2_13::loadData(const std::string & path) {
			std::stringstream suffix;
			suffix << m_num_vars << "Dim.txt";
	
			std::string sa = suffix.str();
			sa.insert(0, m_name + "_a_");
			sa.insert(0, path);
			sa.insert(0, g_working_dir);//probDataPath

			std::string sb = suffix.str();
			sb.insert(0, m_name + "_b_");
			sb.insert(0, path);
			sb.insert(0, g_working_dir);//probDataPath

			std::string salpha = suffix.str();
			salpha.insert(0, m_name + "_alpha_");
			salpha.insert(0, path);
			salpha.insert(0, g_working_dir);//probDataPath

			std::ifstream in_a;
			in_a.open(sa.data());
			std::ifstream in_b;
			in_b.open(sb.data());
			std::ifstream in_alpha;
			in_alpha.open(salpha.data());
			if (in_a.fail()) {
				for (int i = 0; i < m_num_vars; ++i) {
					for (int j = 0; j < m_num_vars; ++j) {
						m_a[i][j] = int(-100.0 + GET_RND(m_id_rnd).uniform.next() * 200);
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

			if (in_b.fail()) {
				for (int i = 0; i < m_num_vars; ++i) {
					for (int j = 0; j < m_num_vars; ++j) {
						m_b[i][j] = int(-100.0 + GET_RND(m_id_rnd).uniform.next() * 200);
					}
				}
				std::ofstream out(sb.c_str());
				for (int i = 0; i < m_num_vars; ++i) {
					for (int j = 0; j < m_num_vars; j++) {
						out << m_b[i][j] << " ";
					}
				}
				out.close();
			}
			else {
				std::string row;
				for (int i = 0; i < m_num_vars; ++i) {
					std::getline(in_b, row);
					std::stringstream sstr_row(row);
					for (int j = 0; j < m_num_vars; j++) {
						sstr_row >> m_b[i][j];
					}
				}
			}
			in_b.close();

			if (in_alpha.fail()) {
				for (int i = 0; i < m_num_vars; ++i) {
					m_alpha[i] = -OFEC_PI + GET_RND(m_id_rnd).uniform.next() * 2 * OFEC_PI;
				}
				std::ofstream out(salpha.c_str());
				for (int i = 0; i < m_num_vars; ++i) {
					out << m_alpha[i] << " ";
				}
				out.close();
			}
			else {
				for (int i = 0; i < m_num_vars; ++i) {
					in_alpha >> m_alpha[i];
				}
			}
			in_alpha.close();
		}

		void Schwefel_2_13::evaluateOriginalObj(Real *x, std::vector<Real> &obj) {
			Real result = 0;
			for (int i = 0; i < m_num_vars; ++i) {
				Real A = 0;
				Real B = 0;
				for (int j = 0; j < m_num_vars; ++j) {
					A += m_a[i][j] * sin(m_alpha[j]) + m_b[i][j] * cos(m_alpha[j]);
					B += m_a[i][j] * sin(x[j]) + m_b[i][j] * cos(x[j]);
				}
				result += pow((A - B), 2.0);
			}
			obj[0] = result + m_bias;
		}
	}
}