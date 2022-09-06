#include "equal_maxima.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
		void EqualMaxima::initialize_() {
			Continuous::initialize_();
			resizeObjective(1);
			auto &v = GET_PARAM(m_id_param);
			resizeVariable(1);
			setDomain(0, 1.);
			m_opt_mode[0] = OptMode::kMaximize;
			m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (ofec::Real)1.e-4;
			m_variable_niche_radius = 1.e-2;
			m_optima.clear();
			VarVec<Real> opt_var(1);
			std::vector<Real> opt_obj(1);
			std::vector<Real> opt_x = { 0.1,0.3,0.5,0.7,0.9 };
			for (Real x : opt_x) {
				opt_var[0] = x;
				evaluateObjective(opt_var.data(), opt_obj);
				m_optima.appendVar(opt_var);
				m_optima.appendObj(opt_obj);
			}
			m_optima.setObjectiveGiven(true);
			m_optima.setVariableGiven(true);
		}

		void EqualMaxima::evaluateObjective(Real *x, std::vector<Real> &obj) {
			Real s;
			s = pow(sin(5 * OFEC_PI*x[0]), 6);
			obj[0] = s;  // note
		}
}