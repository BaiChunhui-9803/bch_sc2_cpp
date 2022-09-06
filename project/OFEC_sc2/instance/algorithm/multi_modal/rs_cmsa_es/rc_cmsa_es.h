//Register RS_CMSA_ES "RS-CMSA-ES" ConOP,GOP,MMOP,SOP

/*
  Ahrari, A., Deb, K., & Preuss, M. (2016). 
  Multimodal Optimization by Covariance Matrix Self-Adaptation Evolution Strategy with Repelling Subpopulations. 
  Evolutionary computation, doi:10.1162/EVCO_a_00182
*/

#ifndef OFEC_RS_CMSA_ES
#define OFEC_RS_CMSA_ES

#include "../../../../core/algorithm/algorithm.h"
#include "../../global/cma_es/cmaes_pop.h"

namespace ofec {
	class RS_CMSA_ES : public Algorithm {
	protected:
		void initialize_()override;
		void run_() override;
		void run_RS_CMSA();

	public:
		void record() override;

	protected:
		enum class DisMetric { Mahalanobis, Euclidean };
		enum class TypeIniC { Eye , Default };
		enum class WhichSubPop { All, Converged };
		size_t m_targetNsubP;					// Default number of subpopulations
		size_t m_initialPop;					// Coefficient for the initial subpopulation
		Real m_TargetNewNiche;					// The desired fraction of identified global minima that should be new basins(The same as alpha_new in the  paper)
		size_t m_DetectMultBudget;				// Max evaluation budget for the hill - valley(DetectMultimodal) function
		Real m_lambdaIncFactor;					// Population size increase factor
		DisMetric m_DisMetric;					// The distance metric.Use 'Mahalanobis' or 'Euclidean'
		Real m_EltRatio;						// The fraction of elite solutions
		Real m_InitializeDisMult;				// Distance multiplier for the intialization
		Real m_MaxIniStepSize;					// The start point for the initial global stepsize
		Real m_TCovCoeff;						// Controls the learning rate for the covariance matrix
		TypeIniC m_iniC;						// 'eye' or 'default'
		Real m_TSigmaCoeff;						// Controls the learning rate for the global Step size
		Real m_TolHistFun;						// For termination andrestarts.Use equal or smaller than the tightest desired function tolerance
		Real m_TargetTolFun;					// Desired function tolerance
		Real m_InidhatPrctile;					// Percentile for the normalized taboo distance of the current subpopulations
		Real m_tolX;							// For termination criteria - not used by default
		Real m_CriticThresh;					// The criticality threshold for taboo points
		Real m_RedRatio;						// Ratio for temporary shrinkage of the taboo regions
		Real m_d0hat;							// initla bvalue for the default normalized taboo distance
		Real m_tau_d;							// Adaptation rate of the normalized taboo distances
		Real m_MuLambdaRatio;					// Fraction of the parents
		Real m_MaxCondC;						// Maximum condition number of the covariance matrix(A termination criterion)
		Real m_ArchiveConsiderTabooTol;			// Tolerance for  being a better solution
		WhichSubPop m_WhichSubPopAnalyze;		// Which subpopulations to be analyzed ? 'all' or 'Converged'
		size_t m_MaxIter;						// The maximum iterations per subpopulation in a restart
		bool m_SaveForResume;					// Allow for resuming the run if interrupted: Use 'yes' or 'no'

		std::vector<Real> m_NormTabDis;			// The normalized taboo distances of the archived solutions (vector)
		std::vector<Solution<>> m_archiveXf;	// archived solutions and their function values (matrix)
		std::list<Solution<>> m_all_converged;	// The best solution of all subpopulations at the end of all restarts (matrix)
		Real m_lambda_exponent;					// Used for increasing popultion size (scalar)
		size_t m_lambda;						// Subpopulation size (Scalar)
		size_t m_restartNo;						// The restart number(scalar)
		size_t m_remained_eval;					// Evaluation budget
		size_t m_NsubP;							// The number of subpopulations in the next restart(scalar)

		size_t m_average_used_iter;
	};
}

#endif // !OFEC_RS_CMSA_ES
