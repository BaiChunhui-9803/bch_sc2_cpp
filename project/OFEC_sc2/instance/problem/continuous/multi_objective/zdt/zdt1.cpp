#include "zdt1.h"

namespace ofec {
	void ZDT1::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real g = 0;
		for (size_t n = 1; n < m_num_vars; n++) {
			g = g + x[n];
		}
		g = 1 + 9 * g / (m_num_vars - 1);
		obj[0] = x[0];
		obj[1] = g * (1 - pow(obj[0] / g, 0.5));
	}
}