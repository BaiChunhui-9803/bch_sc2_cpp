#include "glt2.h"

namespace ofec {
	void GLT2::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real yj = 0 , g = 0;
		for (size_t j = 2; j <= m_num_vars; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_num_vars);
			g += yj * yj;
		}

		obj[0] = (1 + g)*(1 - cos(OFEC_PI*x[0] / 2));
		obj[1] = (1 + g)*(10 - 10 * sin(OFEC_PI*x[0] / 2));
	}
}