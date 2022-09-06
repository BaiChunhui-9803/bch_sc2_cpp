#include"termination.h"
#include "../../utility/typevar/typevar.h"
#include <cmath>

namespace ofec {
	Termination::Termination() : 
		m_max_time(12 * 3600) {}

	bool Termination::terminating() {
		if (!m_enable) return false;
		if (m_isTerminating) return true;
		auto durat = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_start_time).count();
		if (durat >= m_max_time)
			return true;
		return false;
	};

	bool TermMaxEvals::terminating(int evals) {
		if (Termination::terminating()) return true;
		return evals >= m_max_evals;
	}


	bool TermMaxIters::terminating(int value) {
		if (Termination::terminating()) return true;
		return value >= m_max_iters;
	}


	bool TermBestRemain::terminating(Real value) {
		if (Termination::terminating()) return true;
		m_current = value;
		if (m_current != m_previous) {
			m_previous = m_current;
			m_suc_iters = 0;
		}
		else {
			m_suc_iters++;
		}
		return m_suc_iters > m_max_iters;
	}


	bool TermMeanRemain::terminating(Real value) {
		if (Termination::terminating()) return true;
		m_current = value;
		if (fabs(m_previous - m_current)>m_epsilon) {
			m_suc_iters = 0;
			m_previous = m_current;
		}
		else	m_suc_iters++;
		return m_suc_iters >= m_max_iters;
	}

	bool TermVariance::terminating(Real var) {
		if (Termination::terminating()) return true;
		return var < m_epsilon;
	}

	bool TermStagnation::terminating(int value) {
		if (Termination::terminating()) return true;
		if (value == 0) m_cnt++;
		else m_cnt = 0;
		return m_cnt >= m_suc_iters;
	}
}