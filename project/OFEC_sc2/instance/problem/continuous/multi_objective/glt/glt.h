#ifndef OFEC_GLT_H
#define OFEC_GLT_H

#include "../metrics_mop.h"

namespace ofec {
	class GLT : public MetricsMOP {
	protected:
		void initialize_() override;
		void loadParetoFront();
	};
}

#endif

