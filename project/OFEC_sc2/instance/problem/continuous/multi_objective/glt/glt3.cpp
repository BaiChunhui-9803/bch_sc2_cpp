#include "glt3.h"

namespace ofec {
	void GLT3::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real yj, g = 0;
		for (size_t j = 2; j <= m_num_vars; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_num_vars);
			g += yj * yj;
		}

		obj[0] = (1 + g)*x[0];
		if (obj[0] <= 0.05)
			obj[1] = (1 + g)*(1 - 19 * x[0]);
		else
			obj[1] = (1 + g)*(1.0 / 19 - x[0] / 19);
	}
}