#include "encoding.h"
#include "../instance_manager.h"
#include "../../utility/functional.h"

namespace ofec {
	SolBase::SolBase(size_t num_objs, size_t num_cons) : 
		m_obj(num_objs), 
		m_con(num_cons), 
		m_time_evaluate(0) {}

	SolBase::SolBase(const SolBase &sol) :
		m_obj(sol.m_obj), 
		m_con(sol.m_con),
		m_time_evaluate(sol.m_time_evaluate) {}

	SolBase::SolBase(SolBase &&sol) :
		m_obj(std::move(sol.m_obj)),
		m_con(std::move(sol.m_con)),
		m_time_evaluate(sol.m_time_evaluate) {}

	SolBase& SolBase::operator=(const SolBase &rhs) {
		if (this != &rhs) {
			m_obj = rhs.m_obj;
			m_con = rhs.m_con;
			m_time_evaluate = rhs.m_time_evaluate;
		}
		return *this;
	}

	SolBase& SolBase::operator=(SolBase &&rhs) {
		if (this != &rhs) {
			m_obj = std::move(rhs.m_obj);
			m_con = std::move(rhs.m_con);
			m_time_evaluate = rhs.m_time_evaluate;
		}
		return *this;
	}

	void SolBase::initialize(int id_pro, int id_rnd) {
		GET_PRO(id_pro).initSolution(*this, id_rnd);
	}

	Dominance SolBase::compare(const SolBase &s, int id_pro) const {
		return objectiveCompare(m_obj, s.m_obj, GET_PRO(id_pro).optMode());
	}

	bool SolBase::dominate(const std::vector<Real> &o, int id_pro) const {
		return  Dominance::kDominant == objectiveCompare(m_obj, o, GET_PRO(id_pro).optMode());
	}

	bool SolBase::dominate(const SolBase &s, int id_pro) const {
		return Dominance::kDominant == objectiveCompare(m_obj, s.m_obj, GET_PRO(id_pro).optMode());
	}

	Dominance SolBase::compare(const SolBase &s, const std::vector<OptMode> &mode) const {
		return objectiveCompare(m_obj, s.m_obj, mode);
	}

	bool SolBase::dominate(const std::vector<Real> &o, const std::vector<OptMode> &mode) const {
		return  Dominance::kDominant == objectiveCompare(m_obj, o, mode);
	}

	bool SolBase::dominate(const SolBase &s, const std::vector<OptMode> &mode) const {
		return Dominance::kDominant == objectiveCompare(m_obj, s.m_obj, mode);
	}

	bool SolBase::dominEqual(const std::vector<Real> &o, const std::vector<OptMode> &mode) const { //this soluton weakly donimates objective o
		Dominance r = objectiveCompare(m_obj, o, mode);
		return r == Dominance::kDominant || r == Dominance::kEqual;
	}

	bool SolBase::dominEqual(const SolBase &s, const std::vector<OptMode> &mode) const {//this solution weakly donimates solution s
		Dominance r = objectiveCompare(m_obj, s.m_obj, mode);
		return r == Dominance::kDominant || r == Dominance::kEqual;
	}

	bool SolBase::nonDominated(const std::vector<Real> &o, const std::vector<OptMode> &mode) const { //two solutions non-donimate with each other
		return  Dominance::kNonDominated == objectiveCompare(m_obj, o, mode);
	}

	bool SolBase::nonDominated(const SolBase &s, const std::vector<OptMode> &mode) const {//two solutions non-donimate with each other
		return  Dominance::kNonDominated == objectiveCompare(m_obj, s.m_obj, mode);
	}

	bool SolBase::equal(const SolBase &rhs, const std::vector<OptMode> &mode) const {
		return Dominance::kEqual == objectiveCompare(m_obj, rhs.m_obj, mode);
	}

	bool SolBase::same(const SolBase &x, int id_pro) const {//two solutions in decision space
		return GET_PRO(id_pro).same(*this, x);
	}

	Real SolBase::objectiveDistance(const std::vector<Real> &rhs) const {
		return euclideanDistance(rhs.begin(), rhs.end(), m_obj.begin());
	}

	Real SolBase::objectiveDistance(const SolBase &rhs) const {
		return euclideanDistance(rhs.m_obj.begin(), rhs.m_obj.end(), m_obj.begin());
	}

	bool SolBase::boundaryViolated(int id_pro) const {
		return GET_PRO(id_pro).boundaryViolated(*this);
	}

	bool SolBase::constraintViolated(int id_pro) const {
		return GET_PRO(id_pro).constraintViolated(*this);
	}

	size_t SolBase::numViolation() const {
		size_t count = 0;
		for (Real i : m_con)
			if (i > 0) ++count;
		return count;
	}

	void SolBase::setObjInfinite(int id_pro) {
		for (int i = 0; i < m_obj.size(); ++i)
			m_obj[i] = (GET_PRO(id_pro).optMode(i) == OptMode::kMinimize) ? std::numeric_limits<Real>::max() : std::numeric_limits<Real>::min();

	}
	int SolBase::evaluate(int id_pro, int id_alg, bool effective_eval ) {
		int rf = kNormalEval;
		if (ID_ALG_VALID(id_alg) && effective_eval) {
			GET_ALG(id_alg).increaseEffectiveEvals();
			m_time_evaluate = GET_ALG(id_alg).numEffectiveEvals();
			rf = GET_PRO(id_pro).updateEvalTag(*this, id_alg, effective_eval);
			if (rf & kChangeCurEval) {
				if (GET_ALG(id_alg).isObjMinMaxMonitored())
					GET_ALG(id_alg).clearObjMinMax();
				if (GET_ALG(id_alg).keepCandidatesUpdated())
					GET_ALG(id_alg).resetCandidates();
			}
			GET_PRO(id_pro).evaluate(*this, id_alg, effective_eval);
			GET_ALG(id_alg).updateSolved();

			if (GET_ALG(id_alg).keepCandidatesUpdated())
				GET_ALG(id_alg).updateCandidates(*this);
			if (GET_ALG(id_alg).isObjMinMaxMonitored())
				GET_ALG(id_alg).updateObjMinMax(m_obj);

			if (ID_RND_VALID(GET_ALG(id_alg).idRecord())) {
				size_t record_freq = GET_ALG(id_alg).recordFrequency();
				if (m_time_evaluate && m_time_evaluate % record_freq == 0)
					GET_ALG(id_alg).record();
			}
		}
		else {
			GET_PRO(id_pro).evaluate(*this, id_alg, effective_eval);
		}
		return rf;
	}
}