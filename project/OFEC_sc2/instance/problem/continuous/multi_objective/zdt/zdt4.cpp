#include "zdt4.h"

namespace ofec {
	void ZDT4::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real g = 0;
		for (size_t n = 1; n < m_num_vars; n++) {
			g = g + (pow(x[n], 2) - 10 * cos(4 * OFEC_PI * x[n]));
		}
		g = 1 + 10 * (m_num_vars - 1) + g;
		obj[0] = x[0];
		obj[1] = g*(1 - std::sqrt(x[0] / g));
	}
}
