#include "DynDE.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void DynDE::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_r_excl = std::get<Real>(v.at("exlRadius size"));
		m_pop_size = std::get<int>(v.at("population size"));
		m_subpop_size = std::get<int>(v.at("subpopulation size"));
		m_multi_pop.clear();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void DynDE::run_() {
		m_multi_pop.resize(m_pop_size / m_subpop_size, m_subpop_size, m_id_pro);
		for (size_t i = 0; i < m_multi_pop.size(); ++i) {
			m_multi_pop[i].initialize(m_id_pro, m_id_rnd);
			m_multi_pop[i].evaluate(m_id_pro, m_id_alg);
		}
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		int tag = kNormalEval;
		while (!terminating()) {
			for (size_t i = 0; i < m_multi_pop.size(); ++i) {
				if (!m_multi_pop[i].getFlag())
					tag = m_multi_pop[i].evolve(m_id_pro, m_id_alg, m_id_rnd);
				//handle_evaluation_tag(tag);
			}
			if (tag == kTerminate)
				break;
			//exclusion
			exclusion_check();
			for (size_t i = 0; i < m_multi_pop.size(); ++i) {
				if (m_multi_pop[i].getFlag()) {
					m_multi_pop[i].initialize(m_id_pro, m_id_alg);
					m_multi_pop[i].setFlag(false);
				}
			}
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

#ifdef OFEC_DEMO
	void DynDE::updateBuffer() {
		m_solution.resize(m_multi_pop.size());
		for (size_t i = 0; i < m_multi_pop.size(); i++) {
			for (size_t j = 0; j < m_multi_pop[i].size(); ++j)
				m_solution[i].emplace_back(&m_multi_pop[i][j].solut());
		}
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

	void DynDE::exclusion_check() {
		for (size_t i = 0; i < m_multi_pop.size(); ++i) {
			for (size_t j = i + 1; j < m_multi_pop.size(); ++j) {
				if (m_multi_pop[i].getFlag() == false && m_multi_pop[j].getFlag() == false 
					&& m_multi_pop[i].best().front()->variableDistance(*m_multi_pop[j].best().front(), m_id_pro) < m_r_excl)
				{
					if (m_multi_pop[i].best().front()->dominate(*m_multi_pop[j].best().front(), m_id_pro)) {
						m_multi_pop[j].setFlag(true);
					}
					else {
						m_multi_pop[i].setFlag(true);
					}
				}
			}
		}
	}

	void DynDE::record() {
		//if (CONTINUOUS_CAST->has_tag(problem_tag::DOP)) {
		//	// ******* Dynamic Optimization ***********
		//}
		//else if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
		//	// ******* Multi-Modal Optimization *******
		//	size_t evals = CONTINUOUS_CAST->evaluations();
		//	size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
		//	size_t num_opt_known = CONTINUOUS_CAST->getOptima().number_objective();
		//	Real peak_ratio = (Real)num_opt_found / (Real)num_opt_known;
		//	Real success_rate = CONTINUOUS_CAST->solved() ? 1 : 0;
		//	measure::get_measure()->record(global::ms_global.get(), evals, peak_ratio, success_rate);
		//}
		//else if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
		//	// ******* Global Optimization ************
		//	size_t evals = CONTINUOUS_CAST->evaluations();
		//	Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->getOptima().objective(0).at(0));
		//	measure::get_measure()->record(global::ms_global.get(), evals, err);
		//}
	}
}
