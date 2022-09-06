#ifndef OFEC_CMAES_POP_H
#define OFEC_CMAES_POP_H

#include "../../../../core/algorithm/population.h"
#include "../../../../core/algorithm/individual.h"
#include "cmaes.h"

namespace ofec {
	class PopCMAES : public Population<Individual<>> {
	public:
		PopCMAES() = default;
		PopCMAES(size_t size_pop, int id_pro);
		void resize(size_t size, int id_pro);
		void initialize(int id_pro, int id_rnd) override;
		int evaluate(int id_pro, int id_alg) override;
		int evolve(int id_pro, int id_alg, int id_rnd) override;
		~PopCMAES();
		void initializeByNonStd(int id_pro, int id_rnd, const std::vector<Real> &xstart, const std::vector<Real> &stddev);
		void initCMAES(int id_pro);
		void resizePopCMAES(int id_pro);


	protected:
		std::string m_initials_file_path_name;
		cmaes_t m_evo;
		double *m_ar_funvals = nullptr, *const *m_pop = nullptr, *m_xfinal = nullptr;

	protected:
		virtual void reproduce(int id_pro, int id_rnd);
		void normVar(const std::vector<Real> &var, double *x, int id_pro);
		void normDis(const std::vector<Real> &dis, double *s, int id_pro);
		void restoreVar(double *x, std::vector<Real> &var, int id_pro);
		void restoreDis(double *s, std::vector<Real> &dis, int id_pro);
		double mapObj(const Solution<> &s, int id_pro);
	};
}

#endif // !OFEC_CMAES_POP_H