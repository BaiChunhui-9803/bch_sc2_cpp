#ifndef OFEC_SIMULATED_ANNEALING_H
#define OFEC_SIMULATED_ANNEALING_H

#include "../../../../../core/algorithm/algorithm.h"
#include "../../../../../core/instance_manager.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	template<typename TSol>
	class SimulatedAnnealing : public Algorithm {
	public:
		void record();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	protected:
		void initialize_();
		void run_();

		virtual void initSol() = 0;
		virtual void neighbour(const TSol& s, TSol& s_new) = 0;
		virtual Real temperature(Real val);
		virtual Real P(const TSol& s, const TSol& s_new, Real T);

	protected:
		size_t m_k_max;
		std::unique_ptr<TSol> m_s, m_s_new;
	};

	template<typename TSol>
	void SimulatedAnnealing<TSol>::initialize_() {
		Algorithm::initialize_();
		auto& v = GET_PARAM(m_id_param);
		m_k_max = v.count("maximum evaluations") > 0 ? std::get<int>(v.at("maximum evaluations")) : 1e6;
	}

	template<typename TSol>
	void SimulatedAnnealing<TSol>::run_() {
		initSol();
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		size_t k, iter = 0;
		Real T;
		while (!terminating()) {
			k = m_effective_eval;
			T = temperature(static_cast<Real>(k) / m_k_max);
			neighbour(*m_s, *m_s_new);
			m_s_new->evaluate(m_id_pro, m_id_alg);
			if (P(*m_s, *m_s_new, T) >= GET_RND(m_id_rnd).uniform.next())
				*m_s = *m_s_new;
			iter++;
#ifdef OFEC_DEMO
			if (iter % 10 == 0)
				updateBuffer();
#endif
		}
	}

	template<typename TSol>
	void SimulatedAnnealing<TSol>::record() {

	}

	template<typename TSol>
	Real SimulatedAnnealing<TSol>::temperature(Real val) {
		return -log10(val);
	}

	template<typename TSol>
	Real SimulatedAnnealing<TSol>::P(const TSol& s, const TSol &s_new, Real T) {
		if (s_new.dominate(s, m_id_pro))
			return 1.;
		else
			return exp(-abs(s.objective(0) - s_new.objective(0) / T));
	}


#ifdef OFEC_DEMO
	template<typename TSol>
	void SimulatedAnnealing<TSol>::updateBuffer() {
		if (ofec_demo::g_buffer->idAlg() == m_id_alg) {
			m_solution.clear();
			m_solution.resize(1);
			m_solution[0].push_back(m_s.get());
			ofec_demo::g_buffer->appendAlgBuffer(m_id_alg);
		}
	}
#endif
}

#endif // !OFEC_SIMULATED_ANNEALING_H

