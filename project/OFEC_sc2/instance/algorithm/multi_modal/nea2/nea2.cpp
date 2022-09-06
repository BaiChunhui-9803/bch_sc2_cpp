#include "nea2.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/instance_manager.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void NEA2::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_subpops.clear();
	}

	void NEA2::record() {

	}

#ifdef OFEC_DEMO
	void NEA2::updateBuffer() {
		if (m_id_alg == ofec_demo::g_buffer->idAlg()) {
			m_solution.clear();
			m_solution.resize(m_subpops.size());
			for (size_t k = 0; k < m_subpops.size(); k++) {
				for (size_t i = 0; i < m_subpops[k].size(); ++i)
					m_solution[k].push_back(&m_subpops[k][i].phenotype());
			}
			ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
		}
	}
#endif

	void NEA2::run_() {	
		while (!terminating()) {
			addSubpops();
#ifdef OFEC_DEMO
			updateBuffer();
#endif
			while (!terminating() && m_subpops.isActive()) {
				int rf = m_subpops.evolve(m_id_pro, m_id_alg, m_id_rnd);
				if (rf == kTerminate)
					break;
#ifdef OFEC_DEMO
				updateBuffer();
#endif
				for (size_t k = 0; k < m_subpops.size(); ++k) {
					if (m_subpops[k].isActive() && stopTolFun(m_subpops[k]))
						m_subpops[k].setActive(false);
				}
			}

		}
	}

	void NEA2::addSubpops() {
		size_t num_vars = GET_CONOP(m_id_pro).numVariables();
		Population<Individual<>> global_sample(m_pop_size, m_id_pro, num_vars);
		global_sample.initialize(m_id_pro, m_id_rnd);
		global_sample.evaluate(m_id_pro, m_id_alg);
		m_nbc.updateData(global_sample);
		m_nbc.clustering(m_id_pro);
		auto clusters = m_nbc.clusters();
		std::vector<size_t> filtered_start_individuals;
		for (auto& cluster : clusters) {
			auto iter = cluster.begin();
			size_t center = *iter;
			while (++iter != cluster.end()) {
				if (global_sample[*iter].dominate(global_sample[center], m_id_pro))
					center = *iter;
			}
			filtered_start_individuals.push_back(center);
		}
		auto &domain = GET_CONOP(m_id_pro).domain();
		Real diag = 0;
		for (size_t j = 0; j < num_vars; ++j)
			diag += pow(domain[j].limit.second - domain[j].limit.first, 2);
		diag = sqrt(diag);
		Real step_size;
		m_subpops.clear();
		for (size_t start_ind : filtered_start_individuals) {
			step_size = 0.05 * diag;
			//step_size = max(0.025 * diag, global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0.05 * diag, 0.025 * diag));
			auto pop = std::make_unique<PopCMAES>(4 * pow(num_vars, 2), m_id_pro);
			pop->initializeByNonStd(m_id_pro, m_id_rnd, global_sample[start_ind].variable().vect(), std::vector<Real>(num_vars, step_size));
			pop->setId(m_subpops.size());
			m_subpops.append(pop);
		}
	}

	bool NEA2::stopTolFun(PopCMAES& subpop) {
		Real min_obj, max_obj;
		min_obj = max_obj = subpop[0].phenotype().objective(0);
		for (size_t i = 1; i < subpop.size(); i++) {
			if (subpop[i].phenotype().objective(0) < min_obj)
				min_obj = subpop[i].phenotype().objective(0);
			if (subpop[i].phenotype().objective(0) > max_obj)
				max_obj = subpop[i].phenotype().objective(0);
		}
		if ((max_obj - min_obj) < 0.1 * GET_PRO(m_id_pro).objectiveAccuracy())
			return true;
		else
			return false;
	}
}
