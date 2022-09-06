//Register AMP_PSO "AMP-PSO" ConOP,GOP,DOP,MMOP,SOP

#include "../../template/framework/amp/continuous/amp_cont_ind.h"
#include "../../template/framework/amp/continuous/amp_cont.h"
#include "../../../../core/algorithm/algorithm.h"
#include "../../template/classic/pso/particle.h"
#include "../../template/classic/pso/swarm.h"

namespace ofec {
	class AMP_PSO : public Algorithm {
	protected:
		using PopType = Swarm<IndContAMP<Particle>>;
		std::unique_ptr<ContAMP<PopType>> m_multi_pop;
		Real m_w, m_c1, m_c2;
		size_t m_pop_size;

		void initialize_() override;
		void run_() override;
		void initMultiPop();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:
		void record() override;
	};
}
