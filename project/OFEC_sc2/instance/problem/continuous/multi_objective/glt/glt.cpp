#include "glt.h"
#include "../../../../../core/global.h"
#include "../../../../../core/instance_manager.h"
#include <fstream>

namespace ofec {
	void GLT::initialize_() {
		Continuous::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		m_domain.setRange(0, 1, 0);
		for (size_t i = 0; i < m_num_vars; i++)
			m_domain.setRange(-1, 1, i);
		m_domain_update = true;

		resizeObjective(std::get<int>(v.at("number of objectives")));
		if (m_name == "MOP_GLT5" || m_name == "MOP_GLT6") {
			if (m_num_objs != 3)
				throw MyExcept("The number of objectives must be equal to 3.");
		}
		else {
			if (m_num_objs != 2)
				throw MyExcept("The number of objectives must be equal to 2.");
		}
		for (size_t i = 0; i < m_num_objs; ++i) 
			m_opt_mode[i] = OptMode::kMinimize;

		loadParetoFront();		
	}

	void GLT::loadParetoFront(){
		std::ifstream infile;
		std::stringstream os;
		os << g_working_dir << "/instance/problem/continuous/multi_objective/glt/data/" << m_name << ".dat";
		infile.open(os.str());
		if (!infile) {
			throw MyExcept("open PF file of GLT problem is fail");
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