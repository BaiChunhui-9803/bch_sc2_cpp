#include "nsgaiii_sbx.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void NSGAIII_SBX::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_pop.reset();
	}

	void NSGAIII_SBX::initPop() {
		auto size_var = GET_CONOP(m_id_pro).numVariables();
		auto size_obj = GET_CONOP(m_id_pro).numObjectives();
		m_pop.reset(new PopNSGAIII_SBX(m_pop_size, m_id_pro, size_var, size_obj, GET_CONOP(m_id_pro).optMode()));
		m_pop->initialize_(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
	}

	void NSGAIII_SBX::run_() {
		initPop();
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		while (!terminating()) {
			m_pop->evolve(m_id_pro, m_id_alg, m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}


#ifdef OFEC_DEMO
	void NSGAIII_SBX::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1);
		for (size_t i = 0; i < m_pop->size(); ++i)
			m_solution[0].push_back(&m_pop->at(i).phenotype());
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

	void NSGAIII_SBX::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		Real IGD = GET_CONOP(m_id_pro).getOptima().invertGenDist(*m_pop);
		entry.push_back(IGD);
		dynamic_cast<RecordVecReal&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

	PopNSGAIII_SBX::PopNSGAIII_SBX(size_t size_pop, int id_pro, size_t size_var, size_t size_obj, const std::vector<OptMode>& opt_mode) :
		PopSBX<>(size_pop, id_pro), NSGAIII<Individual<>>(size_pop, size_obj, opt_mode, id_pro) {}

	void PopNSGAIII_SBX::initialize_(int id_pro, int id_rnd) {
		initialize(id_pro, id_rnd);
		for (auto& i : m_inds) {
			m_offspring.emplace_back(*i);
		}
		for (auto& i : m_inds) {
			m_offspring.emplace_back(*i);
		}
	}

	int PopNSGAIII_SBX::evolve(int id_pro, int id_alg, int id_rnd) {
		if (m_inds.size() % 2 != 0)
			throw "population size should be even @NSGAIII_SBXRealMu::evolveMO()";
		for (size_t i = 0; i < m_inds.size(); i += 2) {
			std::vector<size_t> p(2);
			p[0] = tournamentSelection(id_pro, id_rnd);
			do { p[1] = tournamentSelection(id_pro, id_rnd); } while (p[1] == p[0]);
			crossover(p[0], p[1], m_offspring[i], m_offspring[i + 1], id_pro, id_rnd);
			mutate(m_offspring[i], id_pro, id_rnd);
			mutate(m_offspring[i + 1], id_pro, id_rnd);
		}
		for (auto& i : m_offspring) {
			i.evaluate(id_pro, id_alg);
		}
		for (size_t i = 0; i < m_inds.size(); ++i) {
			m_offspring[i + m_inds.size()] = *m_inds[i];
		}
		survivorSelection(m_inds, m_offspring, id_pro, id_rnd);


		m_iter++;
		return kNormalEval;
	}
}