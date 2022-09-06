//Register CEC2005_GOP_F12 "GOP_CEC2005_F12" GOP,ConOP,SOP

#ifndef OFEC_F12_SCHWEFEL_2_13_H
#define OFEC_F12_SCHWEFEL_2_13_H

#include "../../../../../core/problem/continuous/function.h"
#include "../metrics_gop.h"

namespace ofec {
	namespace cec2005 {
		class Schwefel_2_13 final: public Function, public MetricsGOP {
		protected:
			void initialize_() override;
			void loadData(const std::string & path);
			void evaluateOriginalObj(Real *x, std::vector<Real>& obj) override;
		private:
			std::vector<std::vector<Real>> m_a;
			std::vector<std::vector<Real>> m_b;
			std::vector<Real> m_alpha;
		};
	}
	using CEC2005_GOP_F12 = cec2005::Schwefel_2_13;
}
#endif // !OFEC_F12_SCHWEFEL_2_13_H
