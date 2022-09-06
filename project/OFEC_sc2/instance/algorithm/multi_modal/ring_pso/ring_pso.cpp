#include "ring_pso.h"
#include "../../../record/rcr_vec_real.h"
#include "../../../../core/instance_manager.h"

namespace ofec {
	void RingParticle::nextVelocity(const Solution<> *lbest, Real w, Real c1, Real c2, int id_rnd) {
		for (size_t j = 0; j < m_var.size(); j++) {
			m_vel[j] = w * (m_vel[j] + 
				c1 * GET_RND(id_rnd).uniform.next() * (m_pbest.variable()[j] - m_var[j]) + 
				c2 * GET_RND(id_rnd).uniform.next() * (lbest->variable()[j] - m_var[j]));
		}
	}

	RingSwarm::RingSwarm(size_t size_pop, Topology topology, int id_pro) : 
		Swarm(size_pop, id_pro),
		m_is_neighbor_set(false),
		m_topology(topology) {}

	void RingSwarm::setNeighborhood(int id_rnd) {
		if (m_is_neighbor_set) 
			return;
		for (size_t i = 0; i < m_inds.size(); ++i) {
			switch (m_topology)
			{
			case Topology::R2:
				m_link[i][i] = true;
				m_link[i][(i + 1) % m_inds.size()] = true;
				break;
			case Topology::R3:
				m_link[i][(i - 1) % m_inds.size()] = true;
				m_link[i][i] = true;
				m_link[i][(i + 1) % m_inds.size()] = true;
				break;
			case Topology::LHC_R2:
				for (size_t j = 0; j < 2 && (i / 2 * 2 + j) < m_inds.size(); ++j)
					m_link[i][(i / 2 * 2 + j)] = true;
				break;
			case Topology::LHC_R3:
				for (size_t j = 0; j < 3 && (i / 3 * 3 + j) < m_inds.size(); ++j)
					m_link[i][(i / 3 * 3 + j)] = true;
				break;
			default:
				break;
			}
		}
		m_is_neighbor_set = true;
	}

	void RingPSO::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_topology = static_cast<RingSwarm::Topology>(std::get<int>(v.at("ring topology")));
		m_w = v.count("weight") > 0 ? std::get<Real>(v.at("weight")) : 0.7298;
		m_c1 = v.count("accelerator1") > 0 ? std::get<Real>(v.at("accelerator1")) : 2.05;
		m_c2 = v.count("accelerator2") > 0 ? std::get<Real>(v.at("accelerator2")) : 2.05;
		m_pop.reset();
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	void RingPSO::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		if (GET_PRO(m_id_pro).hasTag(ProTag::kMMOP)) {
			size_t num_optima_found = GET_PRO(m_id_pro).numOptimaFound(m_candidates);
			size_t num_optima = GET_CONOP(m_id_pro).getOptima().numberObjectives();
			entry.push_back(num_optima_found);
			entry.push_back(num_optima_found == num_optima ? 1 : 0);
		}
		else
			entry.push_back(m_candidates.front()->objectiveDistance(GET_CONOP(m_id_pro).getOptima().objective(0)));
		dynamic_cast<RecordVecReal &>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void RingPSO::updateBuffer() {
		if (ofec_demo::g_buffer->idAlg() == m_id_alg) {
			m_solution.clear();
			m_solution.resize(1);
			for (size_t i = 0; i < m_pop->size(); ++i)
				m_solution[0].push_back(&m_pop->at(i).phenotype());
			ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
		}
	}
#endif

	void RingPSO::run_() {
		initSwarm();
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		while (!this->terminating()) {
			m_pop->evolve(m_id_pro, m_id_alg, m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

	void RingPSO::initSwarm() {
		m_pop.reset(new RingSwarm(m_pop_size, m_topology, m_id_pro));
		m_pop->W() = m_w;
		m_pop->C1() = m_c1;
		m_pop->C2() = m_c2;
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->initVelocity(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
		m_pop->initPbest(m_id_pro);
		m_pop->setNeighborhood(m_id_rnd);
	}
}