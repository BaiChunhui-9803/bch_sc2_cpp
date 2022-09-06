#include "problem.h"
#include "../instance_manager.h"
#include "../../utility/factory.h"

namespace ofec {
	void Problem::initialize() {
		GET_RND(m_id_rnd).initialize();
		initialize_();
		m_initialized = true;
	}

	void Problem::evaluate(SolBase &s, int id_alg, bool effective_eval)	{
		if (!m_initialized)
			throw MyExcept("Problem not initialized.");
		updateCurState(id_alg, effective_eval);
		evaluate_(s, effective_eval);
		++m_total_eval;
	}

	void Problem::initialize_() {
		auto &v = GET_PARAM(m_id_param);
		m_name = std::get<std::string>(v.at("problem name"));
		if (Factory<Problem>::get().count(m_name) > 0)
			m_tag = Factory<Problem>::get().at(m_name).second;
		else m_tag.clear();
		
		m_total_eval = 0; 
		m_num_objs = 0;
		m_num_cons = 0;
		m_objective_accuracy = -1;
		m_variable_accuracy = -1;
		m_variable_niche_radius = -1;
		m_opt_mode.clear();
		m_constraint.clear();
		m_params.clear();
		m_initialized = false;
	}
	
	void Problem::resizeObjective(size_t num_objs) {
		m_num_objs = num_objs;
		m_opt_mode.resize(num_objs);
	}

	void Problem::resizeConstraint(size_t num_cons) {
		m_num_cons = num_cons;
		m_constraint.resize(m_num_cons);
	}

	void Problem::copy(const Problem& rhs) {
		int min_num_objs = std::min(m_num_objs, rhs.m_num_objs);
		int min_num_cons = std::min(m_num_cons, rhs.m_num_cons);
		m_name = rhs.m_name;
		m_num_objs = rhs.m_num_objs;
		m_num_cons = rhs.m_num_cons;

		std::copy(rhs.m_opt_mode.begin(), rhs.m_opt_mode.begin() + min_num_objs, m_opt_mode.begin());
		std::copy(rhs.m_constraint.begin(), rhs.m_constraint.begin() + min_num_cons, m_constraint.begin());


		m_tag = rhs.m_tag;
		m_params = rhs.m_params;
		m_objective_accuracy = rhs.m_objective_accuracy;
		m_variable_accuracy = rhs.m_variable_accuracy;
		m_variable_niche_radius = rhs.m_variable_niche_radius;
	}

	void Problem::updateParameters() {
		m_params["name"] = m_name;
		if (m_num_objs > 0)
			m_params["number of objectives"] = static_cast<int>(m_num_objs);
		if (m_num_cons > 0)
			m_params["number of constraints"] = static_cast<int>(m_num_cons);
		if (m_objective_accuracy > 0)
			m_params["objective accuarcy"] = static_cast<int>(m_objective_accuracy);
		if (m_variable_accuracy > 0)
			m_params["variable accuarcy"] = static_cast<int>(m_variable_accuracy);
		if (m_variable_niche_radius > 0)
			m_params["variable nichie radius"] = static_cast<int>(m_variable_niche_radius);
	}
}