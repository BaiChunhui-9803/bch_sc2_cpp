#include "function.h"
#include <sstream>
#include "../../instance_manager.h"
#include "../../global.h"

namespace ofec {
	void Function::initialize_() {
		Continuous::initialize_();
		m_scaled = false;
		m_rotated = false;
		m_translated = false;
		m_noisy = false;
		m_scale = 0;
		m_bias = 0;
		m_condition_number = 1;
		m_rotation.clear();
		m_translation.clear();
		m_original_optima.clear();
		resizeObjective(1);
	}

	void Function::setBias(Real val) {
		m_bias = val;
	}

	void Function::setScale(Real val) {
		m_scale = val;
		m_scaled = true;
	}

	void Function::setRotated(bool flag) {
		m_rotated = flag;
	}

	void Function::setTranlated(bool flag) {
		m_translated = flag;
	}

	void Function::setScaled(bool flag) {
		m_scaled = flag;
	}

	void Function::setConditionNumber(Real c) {
		m_condition_number = c;
	}

	void Function::clear() {
		m_translation.clear();
	}

	void Function::translateZero() {
		for (auto &i : m_translation)
			i = 0;
	}

	bool Function::loadTranslation(const std::string &path) {
		std::string s;
		std::stringstream ss;
		ss << m_num_vars << "Dim.txt";
		s = ss.str();
		s.insert(0, m_name + "_Shift_");
		s.insert(0, path);    // data path
		s.insert(0, g_working_dir);
		loadTranslation_(s);
		return true;
	}

	void Function::loadTranslation_(const std::string &path) {
		m_translation.resize(m_num_vars);
		std::ifstream in(path);
		if (in.fail()) {
			if (m_original_optima.isVariableGiven())
				setTranslation(m_original_optima.variable(0).data());
			else {
				std::vector<Real> zero(m_num_vars, 0);
				setTranslation(zero.data());
			}
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

	void Function::setTranslation(const Real *opt_var) {
		// Initial the location of shifted global optimum
		m_translation.resize(m_num_vars);
		for (size_t j = 0; j < m_num_vars; ++j) {
			Real x, rl, ru, range;
			x = opt_var[j];
			ru = m_domain[j].limit.second - x;
			rl = x - m_domain[j].limit.first;
			range = rl < ru ? rl : ru;
			m_translation[j] = GET_RND(m_id_rnd).uniform.nextNonStd(-range, range);
		}
		m_translated = true;
	}

	bool Function::loadRotation(const std::string &path) {
		std::string s;
		std::stringstream ss;
		ss << m_num_vars << "Dim.txt";
		s = ss.str();
		s.insert(0, m_name + "_RotM_");

		s.insert(0, path);// data path
		s.insert(0, g_working_dir);

		loadRotation_(s);

		return true;
	}

	void Function::loadRotation_(const std::string &path) {
		m_rotation.resize(m_num_vars, m_num_vars);
		std::ifstream in;
		in.open(path);
		if (in.fail()) {
			setRotation();
			std::ofstream out(path);
			m_rotation.print(out);
			out.close();
		}
		else {
			m_rotation.load(in);
		}
		in.close();
		m_rotated = true;
	}

	void Function::setRotation() {
		m_rotation.generateRotationClassical(&GET_RND(m_id_rnd).normal, m_condition_number);
	}

	void Function::translate(Real *x) {
		for (size_t i = 0; i < m_num_vars; ++i) {
			x[i] -= m_original_optima.variable(0)[i];
		}
		if (m_translated) {
			for (size_t i = 0; i < m_num_vars; ++i) {
				x[i] -= m_translation[i];
			}
		}
	}

	void Function::translateOrigin(Real *x) {
		for (size_t i = 0; i < m_num_vars; ++i) {
			x[i] += m_original_optima.variable(0)[i];
		}
	}

	void Function::rotate(Real *x) {
		if (m_rotated) {
			Real *x_ = new Real[m_num_vars];
			std::copy(x, x + m_num_vars, x_);
			for (size_t i = 0; i < m_num_vars; ++i) {
				x[i] = 0;
				for (size_t j = 0; j < m_num_vars; ++j) {
					x[i] += m_rotation[j][i] * x_[j];
				}
			}
			delete[] x_;
			x_ = 0;
		}
	}

	void Function::scale(Real *x) {
		if (m_scaled) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] /= m_scale;
		}
	}

	void Function::irregularize(Real *x) {
		// this method from BBOB
		Real c1, c2, x_;
		for (size_t i = 0; i < m_num_vars; ++i) {
			if (x[i] > 0) {
				c1 = 10;	c2 = 7.9;
			}
			else {
				c1 = 5.5;	c2 = 3.1;
			}
			if (x[i] != 0) {
				x_ = log(fabs(x[i]));
			}
			else x_ = 0;
			x[i] = sign(x[i]) * exp(x_ + 0.049 * (sin(c1 * x_) + sin(c2 * x_)));
		}
	}

	void Function::asyemmetricalize(Real *x, Real belta) {
		// this method from BBOB
		if (m_num_vars == 1) return;
		for (size_t i = 0; i < m_num_vars; ++i) {
			if (x[i] > 0) {
				x[i] = pow(x[i], 1 + belta * i * sqrt(x[i]) / (m_num_vars - 1));
			}
		}
	}

	void Function::resizeTranslation(size_t n) {
		m_translation.resize(n);
	}

	void Function::resizeRotation(size_t n) {
		m_rotation.resize(n, n);
	}

	void Function::setOriginalGlobalOpt(Real *opt) {
		if (m_num_objs > 1)
			throw MyExcept("Function::setOriginalGlobalOpt only for problems with a single obj");
		m_original_optima.clear();

		VarVec<Real> temp_var(m_num_vars);
		if (opt == 0)		for (auto &i : temp_var) i = 0.;
		else	for (int i = 0; i < m_num_vars; i++)  temp_var[i] = opt[i];
		m_original_optima.appendVar(std::move(temp_var));
		m_original_optima.setVariableGiven(true);

		std::vector<Real> temp_obj(m_num_objs);
		evaluateOriginalObj(temp_var.data(), temp_obj);
		m_original_optima.appendObj(std::move(temp_obj));
		m_original_optima.setObjectiveGiven(true);
	}

	void Function::setGlobalOpt(Real *tran) {
		if (m_num_objs > 1)
			throw MyExcept("Function::setGlobalOpt only for problems with a single obj");
		m_optima.clear();

		VarVec<Real> temp_var(m_num_vars);
		for (int i = 0; i < m_num_vars; ++i) {
			if (tran != 0) temp_var[i] = tran[i] + m_original_optima.variable(0)[i];
			else temp_var[i] = m_original_optima.variable(0)[i];
		}
		m_optima.appendVar(std::move(temp_var));
		m_optima.setVariableGiven(true);

		std::vector<Real> temp_obj(m_num_objs);
		evaluateObjective(temp_var.data(), temp_obj);
		m_optima.appendObj(std::move(temp_obj));
		m_optima.setObjectiveGiven(true);
	}

	void Function::updateParameters() {
		Continuous::updateParameters();
		m_params["scale flag"] = m_scaled;
		m_params["rotation flag"] = m_rotated;
		m_params["translation flag"] = m_translated;
		m_params["noise flag"] = m_noisy;
	}

	void Function::evaluateObjective(Real *x, std::vector<Real> &obj) {
		translate(x);
		scale(x);
		rotate(x);
		translateOrigin(x);
		evaluateOriginalObj(x, obj);
	}
}