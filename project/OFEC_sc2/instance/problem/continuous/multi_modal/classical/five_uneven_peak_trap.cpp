#include "five_uneven_peak_trap.h"
#include "../../../../../core/instance_manager.h"

namespace ofec {
		void FiveUnevenPeakTrap::initialize_() {
			Continuous::initialize_();
			resizeObjective(1);
			m_opt_mode[0] = OptMode::kMaximize;
			auto &v = GET_PARAM(m_id_param);
			resizeVariable(1);
			setDomain(0, 30);
			m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (ofec::Real)1.e-4;
			m_variable_niche_radius = 0.01;
			m_optima.clear();
			std::vector<Real> opt_x = { 0,30 };
			for (Real x : opt_x) {
				VarVec<Real> opt_var(1);
				std::vector<Real> opt_obj(1);
				opt_var[0] = x;
				evaluateObjective(opt_var.data(), opt_obj);
				m_optima.appendVar(opt_var);
				m_optima.appendObj(opt_obj);
			}
			m_optima.setObjectiveGiven(true);
			m_optima.setVariableGiven(true);
		}

		void FiveUnevenPeakTrap::evaluateObjective(Real *x, std::vector<Real> &obj) {
			Real s = -1.0;
			if (x[0] >= 0 && x[0] < 2.5) {
				s = 80 * (2.5 - x[0]);
			}
			else if (x[0] >= 2.5 && x[0] < 5.0) {
				s = 64 * (x[0] - 2.5);
			}
			else if (x[0] >= 5.0 && x[0] < 7.5) {
				s = 64 * (7.5 - x[0]);
			}
			else if (x[0] >= 7.5 && x[0] < 12.5) {
				s = 28 * (x[0] - 7.5);
			}
			else if (x[0] >= 12.5 && x[0] < 17.5) {
				s = 28 * (17.5 - x[0]);
			}
			else if (x[0] >= 17.5 && x[0] < 22.5) {
				s = 32 * (x[0] - 17.5);
			}
			else if (x[0] >= 22.5 && x[0] < 27.5) {
				s = 32 * (27.5 - x[0]);
			}
			else if (x[0] >= 27.5 && x[0] <= 30) {
				s = 80 * (x[0] - 27.5);
			}

			obj[0] = s;
		}
}