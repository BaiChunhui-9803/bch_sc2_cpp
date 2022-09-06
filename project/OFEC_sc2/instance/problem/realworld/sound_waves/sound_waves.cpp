#include "sound_waves.h"

namespace ofec {
	void SoundWaves::initialize_() {
		Continuous::initialize_();
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;

		resizeVariable(6);
		setDomain(-6.4, 6.35);

		m_optima.clear();
		VarVec<Real> opt_var({ 1.0,5.0,1.5,4.8,2.0,4.9 });
		std::vector<Real> opt_obj(1);
		evaluateObjective(opt_var.data(), opt_obj);
		m_optima.appendVar(opt_var);
		m_optima.appendObj(opt_obj);
		m_optima.setVariableGiven(true);
		m_optima.setObjectiveGiven(true);
	}

	void SoundWaves::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real theta = 2 * OFEC_PI / 100.;
		Real s = 0, t;
		for (int i = 0; i < 100; i++) {
			t = x[0] * sin(x[1] * i*theta + x[2] * sin(x[3] * i*theta + x[4] * sin(x[5] * i*theta))) 
				- sin(5.*i*theta + 1.5*sin(4.8*i*theta + 2.0*sin(4.9*i*theta)));
			s += t*t;
		}
		obj[0] = s;
	}
}