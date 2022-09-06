#include "cmaes_pop.h"
#include "../../../../utility/functional.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "cmaes_interface.h"

namespace ofec {
	PopCMAES::PopCMAES(size_t size_pop, int id_pro) :
		Population(size_pop, id_pro, GET_CONOP(id_pro).numVariables()), m_evo() {}

	void PopCMAES::resize(size_t size, int id_pro) {
		Population::resize(size, id_pro, GET_CONOP(id_pro).numVariables());
	}

	void PopCMAES::initialize(int id_pro, int id_rnd) {
		m_initials_file_path_name = g_working_dir + "/instance/algorithm/global/cma_es/initials.par";
		m_ar_funvals = cmaes_init(&m_evo, GET_CONOP(id_pro).numVariables(), 0, 0, 0, m_inds.size(), m_initials_file_path_name.c_str(), id_rnd);
		reproduce(id_pro, id_rnd);
	}

	void PopCMAES::reproduce(int id_pro, int id_rnd) {
		m_pop = cmaes_SamplePopulation(&m_evo, id_rnd);
		for (size_t i = 0; i < m_inds.size(); i++)
			restoreVar(m_pop[i], m_inds[i]->variable().vect(), id_pro);
		/* resample the solution i until it satisfies given	box constraints (variable boundaries) */
		for (size_t i = 0; i < m_inds.size(); ++i) {
			while (GET_PRO(id_pro).boundaryViolated(*m_inds[i])) {
				cmaes_ReSampleSingle(&m_evo, i, id_rnd);
				restoreVar(m_pop[i], m_inds[i]->variable().vect(), id_pro);
			}
		}
	}

	int PopCMAES::evaluate(int id_pro, int id_alg) {
		int tag = kNormalEval;
		m_ar_funvals = cmaes_GetArFunvals(&m_evo);
		for (size_t i = 0; i < m_inds.size(); i++) {
			tag = m_inds[i]->evaluate(id_pro, id_alg);
			if (tag != kNormalEval)
				break;
			m_ar_funvals[i] = mapObj(m_inds[i]->solut(), id_pro);
		}
		return tag;
	}

	int PopCMAES::evolve(int id_pro, int id_alg, int id_rnd) {
		cmaes_UpdateDistribution(&m_evo, m_ar_funvals);
		reproduce(id_pro, id_rnd);
		auto tag = evaluate(id_pro, id_alg);
		m_iter++;
		return tag;
	}

	PopCMAES::~PopCMAES() {
		cmaes_exit(&m_evo); /* release memory */
	}

	void PopCMAES::normVar(const std::vector<Real> &var, double *x, int id_pro) {
		auto &domain = GET_CONOP(id_pro).domain();
		for (size_t j = 0; j < GET_CONOP(id_pro).numVariables(); ++j)
			x[j] = mapReal<double>(var[j], domain[j].limit.first, domain[j].limit.second, 0, 1);
	}

	void PopCMAES::normDis(const std::vector<Real> &dis, double *s, int id_pro) {
		auto &domain = GET_CONOP(id_pro).domain();
		for (size_t j = 0; j < GET_CONOP(id_pro).numVariables(); ++j)
			s[j] = mapReal<double>(dis[j], 0, domain[j].limit.second - domain[j].limit.first, 0, 1);
	}

	void PopCMAES::restoreVar(double *x, std::vector<Real> &var, int id_pro) {
		auto &domain = GET_CONOP(id_pro).domain();
		for (size_t j = 0; j < GET_CONOP(id_pro).numVariables(); ++j)
			var[j] = mapReal<double>(x[j], 0, 1, domain[j].limit.first, domain[j].limit.second);
	}

	void PopCMAES::restoreDis(double *s, std::vector<Real> &dis, int id_pro) {
		auto &domain = GET_CONOP(id_pro).domain();
		for (size_t j = 0; j < GET_CONOP(id_pro).numVariables(); ++j)
			dis[j] = mapReal<double>(s[j], 0, 1, 0, domain[j].limit.second - domain[j].limit.first);
	}

	double PopCMAES::mapObj(const Solution<> &s, int id_pro) {
		if (GET_PRO(id_pro).optMode(0) == OptMode::kMinimize)
			return s.objective()[0];
		else
			return -s.objective()[0];
	}

	void PopCMAES::initializeByNonStd(int id_pro, int id_rnd, const std::vector<Real> &xstart, const std::vector<Real> &stddev) {
		std::vector<double> xstart_(xstart.size()), stddev_(stddev.size());
		normVar(xstart, xstart_.data(), id_pro);
		normDis(stddev, stddev_.data(), id_pro);
		m_ar_funvals = cmaes_init(&m_evo, GET_CONOP(id_pro).numVariables(), xstart_.data(), stddev_.data(), 0, m_inds.size(), 0, id_rnd);
		reproduce(id_pro, id_rnd);
	}

	void PopCMAES::initCMAES(int id_pro) {
		size_t size_pop = m_inds.size();
		m_pop = cmaes_GetPop(&m_evo);
		for (size_t i = 0; i < size_pop; i++)
			normVar(m_inds[i]->variable().vect(), m_pop[i], id_pro);
		m_ar_funvals = cmaes_GetArFunvals(&m_evo);
		for (size_t i = 0; i < size_pop; i++)
			m_ar_funvals[i] = mapObj(m_inds[i]->solut(), id_pro);
		cmaes_InitDistribution(&m_evo, m_ar_funvals);
	}

	void PopCMAES::resizePopCMAES(int id_pro) {
		size_t size_pop = m_inds.size();
		cmaes_ResizeLambda(&m_evo, size_pop);
	}
}