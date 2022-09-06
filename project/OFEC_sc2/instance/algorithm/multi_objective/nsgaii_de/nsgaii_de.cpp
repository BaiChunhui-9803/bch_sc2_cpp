#include "nsgaii_de.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../record/rcr_vec_real.h"
#include <algorithm>

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void NSGAII_DE::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		if (m_pop_size < 5)
			throw "Population size of NSGAIIDE should not be smaller than 5.";
		m_mr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 1.0 / GET_CONOP(m_id_pro).numVariables();
		m_meta = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 20.;
		m_pop.reset();
	}

	void NSGAII_DE::initPop() {
		m_pop.reset(new PopNSGAII_DE(m_pop_size, m_id_pro));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
		m_pop->setParamMODE(m_mr, m_meta);
	}

	void NSGAII_DE::run_() {
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

	void NSGAII_DE::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		Real IGD = GET_CONOP(m_id_pro).getOptima().invertGenDist(*m_pop);
		entry.push_back(IGD);
		dynamic_cast<RecordVecReal&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void NSGAII_DE::updateBuffer() {
		if (ofec_demo::g_buffer->idAlg() == m_id_alg) {
			m_solution.clear();
			m_solution.resize(1);
			for (size_t i = 0; i < m_pop->size(); ++i)
				m_solution[0].push_back(&m_pop->at(i).phenotype());
			ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
		}
	}
#endif

	PopNSGAII_DE::PopNSGAII_DE(size_t size_pop, int id_pro) :
		PopMODE(size_pop, id_pro), 
		NSGAII(NSGAII(GET_PRO(id_pro).numObjectives(), GET_PRO(id_pro).optMode())) ,
		m_pop_combined(2 * size_pop, id_pro, GET_CONOP(id_pro).numVariables()) {}

	int PopNSGAII_DE::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		int m = 0;
		for (int i = 0; i < m_inds.size(); i++) {
			std::vector<size_t> p(3);
			p[0] = tournamentSelection(id_pro, id_rnd);
			while (1) { p[1] = tournamentSelection(id_pro, id_rnd);  	if (p[1] != p[0]) break; }
			while (1) { p[2] = tournamentSelection(id_pro, id_rnd);  	if (p[2] != p[0] && p[2] != p[1]) break; }
			crossMutate(p, m_pop_combined[m], id_pro, id_rnd);
			tag = m_pop_combined[m].evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
			m++;
			m_pop_combined[m++] = *m_inds[i];
		}
		survivorSelection(*this, m_pop_combined);
		m_iter++;
		return tag;
	}
}