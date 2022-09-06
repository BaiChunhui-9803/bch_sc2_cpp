#ifndef OFEC_UF_H
#define OFEC_UF_H

#include"../metrics_mop.h"

namespace ofec {
	class UF : public MetricsMOP {
	protected:
		void initialize_() override;
		void loadParetoFront();
	};
}

#endif
