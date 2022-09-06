#ifndef OFEC_CEC13_COMPOSITION_H
#define OFEC_CEC13_COMPOSITION_H

#include "../../global/CEC2005/composition.h"
#include "../metrics_mmop.h"

namespace ofec {
	namespace cec2013 {
		class Composition : public cec2005::Composition, public MetricsMMOP{
		protected:
			void initialize_() override;
			void evaluateObjective(Real *x, std::vector<Real> &obj) override;
		};
	}
}

#endif // !OFEC_CEC13_COMPOSITION_H