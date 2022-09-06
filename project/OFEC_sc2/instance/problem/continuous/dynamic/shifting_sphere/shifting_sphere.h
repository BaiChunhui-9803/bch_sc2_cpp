//Register ShfitingSphere "Shifting-Sphere" ConOP,DOP,SOP

#include "../../global/classical/sphere.h"

namespace ofec {
	class ShfitingSphere : public Sphere {
	public:
		void initialize_() override;
		int updateEvalTag(SolBase &s, int id_alg, bool effective_eval) override;
	protected:
		
		size_t m_change_freq;
	};
}
