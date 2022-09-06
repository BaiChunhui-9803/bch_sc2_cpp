#include "rc_cmsa_es.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/instance_manager.h"

namespace ofec {
	void RS_CMSA_ES::initialize_() {
		Algorithm::initialize_();
		m_targetNsubP=10;					
		m_initialPop=3;						
		m_TargetNewNiche = 0.5;					
		m_DetectMultBudget = 10;				
		m_lambdaIncFactor = 1.01;				
		m_DisMetric = DisMetric::Mahalanobis;	
		m_EltRatio = 0.15;						
		m_InitializeDisMult = 2;				
		m_MaxIniStepSize = 0.25;				
		m_TCovCoeff = 1;						
		m_iniC = TypeIniC::Default;				
		m_TSigmaCoeff = 1;						
		m_TolHistFun = 1e-5;					
		m_TargetTolFun = 1e-5;					
		m_InidhatPrctile = 25;					
		m_tolX = 0;							
		m_CriticThresh = .01;
		auto D = GET_CONOP(m_id_pro).numVariables();
		m_RedRatio = pow(1.01, 1.0 / D);
		m_d0hat = 1;							
		m_tau_d = pow(1.0 / D, 0.5);
		m_MuLambdaRatio = 0.5;					
		m_MaxCondC = 1e14;						
		m_ArchiveConsiderTabooTol = 0;			
		m_WhichSubPopAnalyze = WhichSubPop::All;
		m_MaxIter = std::numeric_limits<size_t>::max();						
		m_SaveForResume = true;

		m_NormTabDis.clear();		
		m_archiveXf.clear();	
		m_all_converged.clear();
		m_restartNo = 0;
		m_NsubP = m_targetNsubP;
		auto &v = GET_PARAM(m_id_param);
		m_remained_eval = v.count("maximum evaluations") > 0 ? std::get<int>(v.at("maximum evalutation")) : 200000;
	}
	
	void RS_CMSA_ES::run_() {
		auto D = GET_CONOP(m_id_pro).numVariables();
		auto pop0 = m_initialPop * sqrt(D);
		m_lambda = pop0;

		while (!terminating() && m_remained_eval > m_DetectMultBudget * (m_archiveXf.size() + 1) + 2 * m_lambda) {
			m_lambda = floor(pop0 * pow(m_lambdaIncFactor, m_lambda_exponent));
			m_restartNo++;
			run_RS_CMSA();
			auto maxNsubP = floor(m_remained_eval / (m_lambdaIncFactor * m_average_used_iter * m_lambda));
			if (maxNsubP >= m_targetNsubP) {								// Sufficient budget
				m_NsubP = m_targetNsubP;
				m_lambda_exponent += 1;
			}
			else if (maxNsubP >= (m_targetNsubP / m_lambdaIncFactor)) {		// Increase lambda at a slower rate
				m_NsubP = m_targetNsubP;
				auto adjusted_lambda_mult = m_remained_eval / (m_NsubP * m_average_used_iter * m_lambda);
				m_lambda_exponent += log2(adjusted_lambda_mult);
			}
			else if (maxNsubP < (m_targetNsubP / m_lambdaIncFactor))		// Do not increase lambda, reduce NsubP
				m_NsubP = std::max<size_t>(1, floor(m_remained_eval / (m_average_used_iter * m_lambda)));
		}
	}

	void RS_CMSA_ES::run_RS_CMSA() {
		size_t eltNo = ceil(m_lambda * m_EltRatio);
		size_t mu = floor(m_lambda * m_MuLambdaRatio);
		
	}
	
	void RS_CMSA_ES::record()
	{
	}
}