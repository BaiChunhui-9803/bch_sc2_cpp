#include "zdt.h"
#include <fstream>
#include "../../../../../core/instance_manager.h"
#include "../../../../../core/global.h"

namespace ofec {
	void ZDT::initialize_() {
		Continuous::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeObjective(std::get<int>(v.at("number of objectives")));
		if (m_num_objs != 2)
			throw MyExcept("The number of objectives must be equal to 1");
		m_opt_mode[0] = OptMode::kMinimize;
		m_opt_mode[1] = OptMode::kMinimize;

		resizeVariable(std::get<int>(v.at("number of variables")));//recomend n=10
		if (m_num_vars < 2)
			throw MyExcept("The number of variables must be no less than 2");	
		if (m_name == "MOP_ZDT4") {
			m_domain.setRange(0., 1., 0);
			for (size_t i = 1; i < m_num_vars; ++i)
				m_domain.setRange(-5., 5., i);
			m_domain_update = true;
		}
		else
			setDomain(0., 1.);

		generateAdLoadPF();
	}

	void ZDT::generateAdLoadPF() {
		size_t num = 1000;
		m_optima.resizeObjectiveSet(num);
		m_optima.resizeVariableSet(num);

		std::ofstream out;
		std::ifstream infile;
		std::stringstream os;
		os << g_working_dir << "/instance/problem/continuous/multi_objective/zdt/data/PF_" << m_name << "_Opt_" << num << ".txt";
		infile.open(os.str());

		if (!infile) {
			out.open(os.str());
			Real temp;
			for (size_t i = 0; i < num; i++) {
				VarVec<Real> temp_var(m_num_vars);
				std::vector<Real> temp_obj(m_num_objs);
				temp = static_cast<Real>(i) / num;
				temp_var[0] = temp;
				for (size_t j = 1; j < m_num_vars; j++)
					temp_var[j] = 0.;
				evaluateObjective(temp_var.data(), temp_obj);
				m_optima.setVariable(temp_var, i);
				m_optima.setObjective(temp_obj, i);
				out << temp_obj[0] << " " << temp_obj[1] << std::endl;
			}
			out.close();
		}
		else {
			Real temp;
			for (size_t i = 0; i < num; i++) {
				VarVec<Real> temp_var(m_num_vars);
				std::vector<Real> temp_obj(m_num_objs);
				temp = static_cast<Real>(i) / num;
				temp_var[0] = temp;
				for (size_t j = 1; j < m_num_vars; j++)
					temp_var[j] = 0.;
				infile >> temp_obj[0];
				infile >> temp_obj[1];
				m_optima.setVariable(temp_var, i);
				m_optima.setObjective(temp_obj, i);
			}
			infile.close();
		}

		m_optima.setObjectiveGiven(true);
		m_optima.setVariableGiven(true);
	}
}