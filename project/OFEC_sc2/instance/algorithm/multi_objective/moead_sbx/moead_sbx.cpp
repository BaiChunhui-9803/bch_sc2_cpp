#include "moead_sbx.h"
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void MOEAD_SBX::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_pop.reset();
	}

	void MOEAD_SBX::initPop() {
		auto size_var = GET_CONOP(m_id_pro).numVariables();
		auto size_obj = GET_CONOP(m_id_pro).numObjectives();
		m_pop.reset(new PopMOEAD_SBX(m_pop_size, m_id_pro));
		m_pop->initialize_(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
	}

	void MOEAD_SBX::run_() {
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

	void MOEAD_SBX::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		Real IGD = GET_CONOP(m_id_pro).getOptima().invertGenDist(*m_pop);
		entry.push_back(IGD);
		dynamic_cast<RecordVecReal&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void MOEAD_SBX::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1);
		for (size_t i = 0; i < m_pop->size(); ++i)
			m_solution[0].push_back(&m_pop->at(i).phenotype());
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

	PopMOEAD_SBX::PopMOEAD_SBX(size_t size_pop, int id_pro) :
		PopSBX<>(size_pop, id_pro), MOEAD<Individual<>>(id_pro) {}

	void PopMOEAD_SBX::initialize_(int id_pro, int id_rnd) {
		Population<Individual<>>::initialize(id_pro, id_rnd);
		MOEAD<Individual<>>::initialize(this->m_inds, id_pro);
	}

	int PopMOEAD_SBX::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		std::vector<int> perm(this->m_inds.size());
		for (int i(0); i < perm.size(); ++i) {
			perm[i] = i;
		}
		GET_RND(id_rnd).uniform.shuffle(perm.begin(), perm.end());
		for (int i = 0; i < this->m_inds.size(); i += 2) {
			int n = perm[i];
			// or int n = i;
			int type = 1; //only select parent individual from neighborhood
			double rnd = GET_RND(id_rnd).uniform.next();

			// select the indexes of mating parents
			std::vector<int> p;
			matingSelection(p, n, 1, type, this->m_inds.size(), id_rnd);  // neighborhood selection

			//produce a child Individual
			Individual<> child1 = *this->m_inds[0];
			Individual<> child2 = *this->m_inds[0];
			std::vector<size_t> index(2);
			index[0] = n; index[1] = p[0];
			this->crossover(index[0], index[1], child1, child2, id_pro, id_rnd);
			mutate(child1, id_pro, id_rnd);
			mutate(child2, id_pro, id_rnd);
			tag = child1.evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
			tag = child2.evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
			// update the reference points and other TypeIndivs in the neighborhood or the whole population
			updateReference(child1, id_pro);
			updateReference(child2, id_pro);
			updateProblem(this->m_inds, child1, n, type, id_pro, id_rnd);
			updateProblem(this->m_inds, child2, n, type, id_pro, id_rnd);
		}
		m_iter++;
		return tag;
	}
}
