#include "CEC2015_function.h"

namespace ofec {
	namespace CEC2015 {
		CEC2015_function::CEC2015_function(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), 
			function(name, size_var, size_obj)
		{
			setDomain(-100, 100);
			setInitialDomain(-100, 100);
			
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			setConditionNumber(1.0);
			m_variable_monitor = true;
		}
		
		bool CEC2015_function::loadTranslation(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << ".txt";
			s = ss.str();
			s.insert(0, m_name + "_Shift");
			s.insert(0, path);    // data path
			s.insert(0, g_working_dir);

			load_translation_(s);

			return true;
		}
		void CEC2015_function::load_translation_(const std::string &path) {
			m_translation.resize(m_num_vars);
			std::ifstream in(path);
			if (in.fail()) {
				
				std::vector<Real> temp(m_num_vars, 20);
				setTranslation(temp.data());
				
				std::ofstream out(path);
				for (auto &i : m_translation)
					out << i << " ";
				out.close();
			}
			else {
				for (auto &i : m_translation)
					in >> i;
			}
			in.close();
			m_translated = true;
		}

		
		bool CEC2015_function::load_optima(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << m_num_vars << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_Optima_");

			s.insert(0, path);// data path
			s.insert(0, g_working_dir);

			load_optima_(s);

			return true;
		}

		void CEC2015_function::load_optima_(const std::string &path) {
			std::ifstream in;
			in.open(path);
			if (in.fail()) {
				set_optima();
				std::ofstream out(path);
				for (size_t i = 0; i < m_optima.number_variable();++i)
					for (size_t j = 0; j < m_num_vars; ++j)
						out << m_optima.variable(i)[j] << " ";
				out.close();
			}
			else {
				while(!(in.eof())) {
					VarVec<Real> temp_var(m_num_vars);
					for (size_t j = 0; j < m_num_vars; ++j)
						in >> temp_var[j];
					m_optima.append(std::move(temp_var));
				}
			}
			in.close();
		}

		void CEC2015_function::set_optima() {
			//  to do ..
			throw myexcept("Waiting to do !");
		}

		void CEC2015_function::evaluate_optima(){
            Solution<VarVec<Real>, Real> s(m_num_objs, num_constraints(), m_num_vars);
            for (size_t i = 0; i < m_optima.number_variable(); ++i) {
			    s.variable() = m_optima.variable(i);
			    s.evaluate(false, caller::Problem);
				m_optima.append(s.objective());
			}
			
		}
		void CEC2015_function::rotate(Real *x) {
			Real *x_ = new Real[m_num_vars];
			std::copy(x, x + m_num_vars, x_);

			for (size_t i = 0; i<m_num_vars; ++i) {
				x[i] = 0;

				for (size_t j = 0; j < m_num_vars; ++j) {
					x[i] += m_rotation[i][j] * x_[j];
				}
			}

			delete[] x_;
			x_ = 0;
		}
	}
}