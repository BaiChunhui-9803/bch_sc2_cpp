#include "nsgaii_sbx.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void NSGAII_SBX::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		if (m_pop_size % 2)
			throw MyExcept("Population size of NSGAII should be even.");
		m_cr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.9;
		m_mr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 1.0 / GET_CONOP(m_id_pro).numVariables();
		m_ceta = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 20.;
		m_meta = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 20.;
	}

	void NSGAII_SBX::initPop() {
		m_pop.reset(new PopNSGAII_SBX(m_pop_size, m_id_pro));
		m_pop->setCR(m_cr);
		m_pop->setMR(m_mr);
		m_pop->setEta(m_ceta, m_meta);
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
	}

	void NSGAII_SBX::run_() {
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

	void NSGAII_SBX::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		Real IGD = GET_CONOP(m_id_pro).getOptima().invertGenDist(*m_pop);
		entry.push_back(IGD);
		dynamic_cast<RecordVecReal&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void NSGAII_SBX::updateBuffer() {
		if (ofec_demo::g_buffer->idAlg() == m_id_alg) {
			m_solution.clear();
			m_solution.resize(1);
			for (size_t i = 0; i < m_pop->size(); ++i)
				m_solution[0].push_back(&m_pop->at(i).phenotype());
			ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
		}
	}
#endif

	/* ============== population ============== */

	PopNSGAII_SBX::PopNSGAII_SBX(size_t size_pop, int id_pro) :
		PopSBX<>(size_pop, id_pro),
		NSGAII(GET_PRO(id_pro).numObjectives(), GET_PRO(id_pro).optMode()),
		m_pop_combined(size_pop * 2, id_pro, GET_CONOP(id_pro).numVariables()) {}

	int PopNSGAII_SBX::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		for (size_t i = 0; i < m_inds.size(); i += 2) {
			std::vector<size_t> p(2);
			p[0] = tournamentSelection(id_pro, id_rnd);
			do { p[1] = tournamentSelection(id_pro, id_rnd); } while (p[1] == p[0]);
			crossover(p[0], p[1], m_pop_combined[i], m_pop_combined[i + 1], id_pro, id_rnd);
			mutate(m_pop_combined[i], id_pro, id_rnd);
			mutate(m_pop_combined[i + 1], id_pro, id_rnd);
		}
		for (size_t i = 0; i < this->m_inds.size(); ++i) {
			tag = m_pop_combined[i].evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
		}
		for (size_t i = 0; i < this->m_inds.size(); ++i) {
			m_pop_combined[i + this->m_inds.size()] = *this->m_inds[i];
		}
		survivorSelection(*this, m_pop_combined);
		m_iter++;
		return tag;
	}
}