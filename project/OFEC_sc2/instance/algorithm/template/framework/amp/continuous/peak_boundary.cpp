#include "peak_boundary.h"
#include "../../../../../../core/problem/continuous/continuous.h"
#include <algorithm>
#include "../../../../../../core/instance_manager.h"

namespace ofec {
	PeakBoundary::PeakBoundary(int num_vars, int num_objs) :
		Solution(num_vars, num_objs),
		m_radius(0), 
		m_start_radius(0),
		m_vbest(num_vars),
		m_step(0), 
		m_ready(false),
		m_order(1), 
		m_derating_obj(0) {}

	PeakBoundary::PeakBoundary(const Solution& s, int id_pro, int id_alg, int id_rnd, Real startRadi, Real step) :
		Solution<>(s), 
		m_radius(0), 
		m_start_radius(startRadi), 
		m_vbest(s.variable().vect()), 
		m_step(step),
		m_ready(false), 
		m_order(1)
	{
		m_derating_obj = GET_PRO(id_pro).optMode(0) == OptMode::kMinimize ?
			GET_ALG(id_alg).maxObjFound(0) :
			GET_ALG(id_alg).minObjFound(0);
		if (m_start_radius == 0) {
			m_start_radius = GET_CONOP(id_pro).variableAccuracy() / 3;
		}
		if (m_step == 0) {
			m_step = m_start_radius / 3;
		}
		generateSample(id_pro, id_alg, id_rnd);
	}

	void PeakBoundary::setRadius(Real r) {
		m_radius = r;
	}

	Real PeakBoundary::getRadius() {
		return m_radius;
	}

	void PeakBoundary::setDeratingObj(int num) {
		m_order = num;
	}

	Real PeakBoundary::getDeratingObj(int id_pro, int id_alg) {
		Real w = pow(1.001, -sqrt(m_order));
		if (GET_PRO(id_pro).optMode(0) == OptMode::kMinimize)
			m_derating_obj = GET_ALG(id_alg).minObjFound(0) * (1 - w) + GET_ALG(id_alg).maxObjFound(0) * w;
		else
			m_derating_obj = GET_ALG(id_alg).maxObjFound(0) * (1 - w) + GET_ALG(id_alg).minObjFound(0) * w;
		return m_derating_obj;
	}

	Real PeakBoundary::getRefRadi(const Solution<>& s, int id_pro, int id_alg, int id_rnd) {
		if (!m_ready) return 0; 
		Vector v(s.variable().vect());
		v -= m_vbest;
		if (v.length() == 0) return m_radius;
		std::vector<Real> angle;
		for (auto& i : m_sample) {
			angle.push_back(v.angle(i));
		}
		int idx = min_element(angle.begin(), angle.end()) - angle.begin();
		Real mang = angle.empty() ? OFEC_PI : angle[idx];
		if (mang > 3 * OFEC_PI / 180) {
			m_ready = false;
			creatOneSample(v, id_pro, id_alg, id_rnd);
			m_ready = true;
			return m_sample.back().length();
		}
		return m_sample[idx].length();
	}

	void PeakBoundary::generateSample(int id_pro, int id_alg, int id_rnd) {
		int dim = m_var.size();
		for (int tr = 0; tr < dim * 2; ++tr) {
			Vector vnor(dim);
			vnor.randomize(&GET_RND(id_rnd).uniform, -1, 1);
			vnor.normalize();
			creatOneSample(vnor, id_pro, id_alg, id_rnd);
		}
		m_ready = true;
	}

	void PeakBoundary::creatOneSample(const Vector& vnor, int id_pro, int id_alg, int id_rnd) {
		Vector vtr(vnor);
		Solution<> s0(*this), s1(*this);
		Real r = m_start_radius;
		bool flag = true, first = true;
		Real dr0 = 1, dr1 = 0;
		do {
			s0 = s1;
			vtr = vnor * r;
			vtr += m_vbest;
			copy(vtr.begin(), vtr.end(), s1.variable().begin());
			if (!s1.boundaryViolated(id_pro)) {
				s1.evaluate(id_pro, id_alg);
			}
			else {
				GET_PRO(id_pro).validateSolution(s1, Validation::kSetToBound, id_rnd);
				//	SolutionValidation mode = VALIDATION_SETTOBOUND;
				//	s1.validate(&mode);
				s0 = s1;
				flag = false;
				break;
			}
			//June 19, 2016 TO DO: need to test
			/*if (dynamic_cast<FreePeak*>(Global::msp_global->mp_problem.get())) {
				dr1 = s0.getObjDistance_(s1.data().m_obj);
				if (!first) {
					if (dr1 / dr0 > 10) {
						flag = false;
						break;
					}
				}
				dr0 = dr1;
				if (first) first = false;
			}*/
			//***********
			r += m_step;
		} while (s1.dominate(s0, GET_PRO(id_pro).optMode()));
		if (flag) {
			binarySearch(s0, s1, id_pro, id_alg);
		}
		Vector vr(s0.variable().vect());
		vr -= m_vbest;
		m_sample.push_back(std::move(vr));
		if (m_sample.size() == 1 || m_radius > r) m_radius = r;
	}

	PeakBoundary& PeakBoundary::operator=(const PeakBoundary& rhs) {
		Solution::operator=(rhs);
		m_radius = rhs.m_radius;
		m_sample = rhs.m_sample;
		m_start_radius = rhs.m_start_radius;
		m_vbest = rhs.m_vbest;
		m_derating_obj = rhs.m_derating_obj;
		m_step = rhs.m_step;
		m_ready = rhs.m_ready;
		m_order = rhs.m_order;
		return *this;
	}

	PeakBoundary::PeakBoundary(const PeakBoundary& rhs) :Solution(rhs) {
		m_radius = rhs.m_radius;
		m_sample = rhs.m_sample;
		m_start_radius = rhs.m_start_radius;
		m_vbest = rhs.m_vbest;
		m_derating_obj = rhs.m_derating_obj;
		m_step = rhs.m_step;
		m_ready = rhs.m_ready;
		m_order = rhs.m_order;
	}

	Vector& PeakBoundary::operator[](int i) {
		return m_sample[i];
	}

	int PeakBoundary::size() {
		return m_sample.size();
	}

	Vector& PeakBoundary::getBest() {
		return m_vbest;
	}

	void PeakBoundary::binarySearch(Solution<>& s0, Solution<>& s1, int id_pro, int id_alg) {
		Solution<> s(s0);
		for (int i = 0; i < s.variable().size(); ++i) {
			s.variable()[i] = (s0.variable()[i] + s1.variable()[i]) / 2.;
		}
		s.evaluate(id_pro, id_alg);
		if (s0.variableDistance(s, id_pro) <= 1.e-3) {
			s0 = s;
			return;
		}
		if (s0.dominate(s, GET_PRO(id_pro).optMode())) 
			binarySearch(s, s1, id_pro, id_alg);
		if (s.dominate(s0, GET_PRO(id_pro).optMode()))
			binarySearch(s0, s, id_pro, id_alg);
	}
}
