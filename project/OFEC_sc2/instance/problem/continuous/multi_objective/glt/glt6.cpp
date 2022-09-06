#include "glt6.h"

namespace ofec {
	void GLT6::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real yj, g = 0;
		for (size_t j = 3; j <= m_num_vars; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_num_vars);
			g += yj * yj;
		}

		obj[0] = (1 + g)*(1 - cos((x[0] / 2)*OFEC_PI))*(1 - cos((x[1] / 2)*OFEC_PI));
		obj[1] = (1 + g)*(1 - cos((x[0] / 2)*OFEC_PI))*(1 - sin((x[1] / 2)*OFEC_PI));
		obj[2] = (1 + g)*(2 - sin((x[0] / 2)*OFEC_PI) - sign(cos(4 * x[0] * OFEC_PI)));
	}
}