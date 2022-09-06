#include "algorithm.h"
#include "../instance_manager.h"
#include "../problem/problem.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void Algorithm::initialize() {
		GET_RND(m_id_rnd).initialize();
		initialize_();
		if (m_obj_minmax_monitored) {
			m_minmax_objective.resize(GET_PRO(m_id_pro).numObjectives());
			for (auto &i : m_minmax_objective) {
				i[0] = std::numeric_limits<Real>::max();
				i[1] = std::numeric_limits<Real>::min();
			}
		}
		m_initialized = true;
	}

	void Algorithm::run() {
		if (!m_initialized)
			throw MyExcept("Algorithm not initialized.");
		run_();
		if (ID_RCR_VALID(m_id_rcr))
			record();
		m_term->setTerminatedTrue();
	}

	bool Algorithm::terminated() {
		return m_term->terminated();
	}

	bool Algorithm::terminating() {
#ifdef OFEC_DEMO
		if (ofec_demo::g_term_alg)
#else 
		if (solved())
#endif
			return true;
		if (auto t = dynamic_cast<TermMaxEvals*>(m_term.get())) //by default 
			return t->terminating(m_effective_eval);
		else
			return m_term->terminating();
	}

	Real Algorithm::duration() {
		return m_term->duration();
	}

	void Algorithm::setTermination(Termination *term) {
		m_term.reset(term);
	}

	void Algorithm::updateCandidates(const SolBase &sol) {
		GET_PRO(m_id_pro).updateCandidates(sol, m_candidates);
	}

	void Algorithm::updateSolved() {
		m_solved = GET_PRO(m_id_pro).isSolved(m_candidates);
	}

	void Algorithm::initialize_() {
		auto &v = GET_PARAM(m_id_param);
		m_name = std::get<std::string>(v.at("algorithm name"));
		m_record_frequency = v.count("sample frequency") > 0 ? std::get<int>(v.at("sample frequency")) : 100;
		m_term.reset(v.count("maximum evaluations") > 0 ? new TermMaxEvals(std::get<int>(v.at("maximum evaluations"))) : new Termination);
		m_effective_eval = 0;
		m_solved = false;
		m_minmax_objective.clear();
		m_obj_minmax_monitored = false;
		m_candidates.clear();
		m_keep_candidates_updated = false;
	}

	void Algorithm::updateObjMinMax(const std::vector<Real> &obj) {
		if (m_minmax_objective.size() < GET_PRO(m_id_pro).numObjectives()) {
			int before_obj(m_minmax_objective.size());
			m_minmax_objective.resize(GET_PRO(m_id_pro).numObjectives());
			for (int obj_idx(before_obj); obj_idx < GET_PRO(m_id_pro).numObjectives(); ++obj_idx) {
				m_minmax_objective[obj_idx][0] = std::numeric_limits<Real>::max();
				m_minmax_objective[obj_idx][1] = std::numeric_limits<Real>::min();
			}
		}
		else if (m_minmax_objective.size() > GET_PRO(m_id_pro).numObjectives()) {
			m_minmax_objective.resize(GET_PRO(m_id_pro).numObjectives());
		}
		for (int obj_idx = 0; obj_idx < GET_PRO(m_id_pro).numObjectives(); ++obj_idx) {
			if (m_minmax_objective[obj_idx][0] > obj[obj_idx]) {
				m_minmax_objective[obj_idx][0] = obj[obj_idx];
			}
			if (m_minmax_objective[obj_idx][1] < obj[obj_idx]) {
				m_minmax_objective[obj_idx][0] = obj[obj_idx];
			}
		}
	}
	void Algorithm::clearObjMinMax()
	{
		m_minmax_objective.resize(GET_PRO(m_id_pro).numObjectives());
		for (int obj_idx(0); obj_idx < GET_PRO(m_id_pro).numObjectives(); ++obj_idx) {
			m_minmax_objective[obj_idx][0] = std::numeric_limits<Real>::max();
			m_minmax_objective[obj_idx][1] = std::numeric_limits<Real>::min();
		}
	}
}
