//Register UnevenDeMaxima "Classic_uneven_de_maxima" MMOP,ConOP,SOP

//Beasley's F4 function

#ifndef OFEC_UNEVEN_DE_MAXIMA_H
#define OFEC_UNEVEN_DE_MAXIMA_H

#include "../metrics_mmop.h"

namespace ofec {
	class UnevenDeMaxima : public MetricsMMOP {
	protected:
		void initialize_() override;
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	};	
}
#endif // !OFEC_UNEVEN_DE_MAXIMA_H