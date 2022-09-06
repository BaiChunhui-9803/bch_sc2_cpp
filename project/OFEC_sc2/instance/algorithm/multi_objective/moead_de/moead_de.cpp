#include "moead_de.h"
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	void MOEAD_DE::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_pop.reset();
	}

	void MOEAD_DE::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		Real IGD = GET_CONOP(m_id_pro).getOptima().invertGenDist(*m_pop);
		entry.push_back(IGD);
		dynamic_cast<RecordVecReal&>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void MOEAD_DE::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1);
		for (size_t i = 0; i < m_pop->size(); ++i)
			m_solution[0].push_back(&m_pop->at(i).phenotype());
		ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

	void MOEAD_DE::initPop() {
		auto size_var = GET_CONOP(m_id_pro).numVariables();
		auto size_obj = GET_CONOP(m_id_pro).numObjectives();
		m_pop.reset(new PopMOEAD_DE(m_pop_size, m_id_pro));
		m_pop->initialize_(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
	}

	void MOEAD_DE::run_() {
		initPop();
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		while (!terminating()) {
			m_pop->evolve(m_id_pro,m_id_alg,m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

	PopMOEAD_DE::PopMOEAD_DE(size_t size_pop, int id_pro) :
		PopMODE<>(size_pop,id_pro), MOEAD<IndDE>(id_pro) { }

	void PopMOEAD_DE::initialize_(int id_pro, int id_rnd) {
		Population<IndDE>::initialize(id_pro,id_rnd);
		MOEAD<IndDE>::initialize(this->m_inds, id_pro);
	}

	int PopMOEAD_DE::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		std::vector<int> perm(this->m_inds.size());
		for (int i(0); i < perm.size(); ++i) {
			perm[i] = i;
		}
		GET_RND(id_rnd).uniform.shuffle(perm.begin(), perm.end());
		for (int i = 0; i < this->m_inds.size(); i++) {
			int n = perm[i];
			// or int n = i;
			int type;
			double rnd = GET_RND(id_rnd).uniform.next();
			// mating selection based on probability
			if (rnd < m_realb)
				type = 1;   // neighborhood
			else
				type = 2;   // whole population

			// select the indexes of mating parents
			std::vector<int> p;
			matingSelection(p, n, 2, type, this->m_inds.size(), id_rnd);  // neighborhood selection

			// produce a child Individual
			ofec::IndDE child = *this->m_inds[0];
			std::vector<size_t> index(3);
			index[0] = n; index[1] = p[0]; index[2] = p[1];
			this->crossMutate(index, child, id_pro, id_rnd);
			tag = child.evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
			// update the reference points and other TypeIndivs in the neighborhood or the whole population
			updateReference(child, id_pro);
			updateProblem(this->m_inds, child, n, type, id_pro, id_rnd);
		}
		m_iter++;
		return tag;
	}
}
