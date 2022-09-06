#include "uf.h"
#include <fstream>
#include "../../../../../core/instance_manager.h"
#include "../../../../../core/global.h"

namespace ofec {
	void UF::initialize_() {
		Continuous::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));  //recomend n=10
		if (m_name == "MOP_UF03")
			setDomain(0., 1.);
		else {
			if (m_name == "MOP_UF04") {
				m_domain.setRange(0., 1., 0);
				for (size_t i = 1; i < m_num_vars; ++i)
					m_domain.setRange(-2., 2., i);
			}
			else if (m_name == "MOP_UF08" || m_name == "MOP_UF09" || m_name == "MOP_UF10") {
				for (size_t i = 0; i < m_num_vars; ++i) {
					if (i == 0 || i == 1)
						m_domain.setRange(0., 1., i);
					else
						m_domain.setRange(-2., 2., i);
				}
			}
			else {
				m_domain.setRange(0., 1., 0);
				for (size_t i = 1; i < m_num_vars; ++i)
					m_domain.setRange(-1., 1., i);
			}
			m_domain_update = true;
		}

		resizeObjective(std::get<int>(v.at("number of objectives")));
		if (m_name == "MOP_UF08" || m_name == "MOP_UF09" || m_name == "MOP_UF10") {
			if (m_num_objs != 3)
				throw MyExcept("The number of objectives must be equal to 3.");
			if (m_num_vars < 3)
				throw MyExcept("The number of variables must be no less than 3.");
		}
		else {
			if (m_num_objs != 2)
				throw MyExcept("The number of objectives must be equal to 2.");
			if (m_name == "MOP_UF03" && m_num_vars < 3)
				throw MyExcept("The number of variables must be no less than 3.");
		}
		for (size_t i = 0; i < m_num_objs; ++i)
			m_opt_mode[i] = OptMode::kMinimize;

		loadParetoFront();
	}

	void UF::loadParetoFront() {
		std::stringstream os;
		os << g_working_dir << "/instance/problem/continuous/multi_objective/uf/data/" << m_name << ".dat";
		std::ifstream infile(os.str());
		if (!infile) {
			//std::cout << "open file is failed" << std::endl;
			throw "open PF file of UF problem is fail";
			return;
		}
		std::string str;
		size_t line = 0;
		while (getline(infile, str))
			++line;
		m_optima.resizeObjectiveSet(line);
		infile.close();
		infile.clear();
		infile.open(os.str());
		for (size_t i = 0; i < line; i++) {
			std::vector<Real> temp_obj(m_num_objs);
			for (size_t j = 0; j < m_num_objs; j++)
				infile >> temp_obj[j];
			m_optima.setObjective(temp_obj, i);
		}
		m_optima.setObjectiveGiven(true);
		infile.close();
	}
}