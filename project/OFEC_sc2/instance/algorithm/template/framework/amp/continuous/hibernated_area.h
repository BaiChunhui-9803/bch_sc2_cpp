#ifndef HIBERNATED_AREA_H
#define HIBERNATED_AREA_H

#include "peak_boundary.h"
#include <climits>

namespace ofec {
	class HibernatedArea {
	protected:
		std::vector<PeakBoundary> m_found_optimums;
	public:
		HibernatedArea() = default;
	  
		void add_optimum(const Solution<> & sol, int id_pro, int id_alg, int id_rnd) {
			m_found_optimums.emplace_back(sol, id_pro, id_alg, id_rnd);
		}

		PeakBoundary & operator[](int idx) {
			return m_found_optimums[idx];
		}
	
		void get_nearest_optimum(const Solution<> & sol, int& idx, Real& minDis, int id_pro) {
			idx = -1;
			minDis = std::numeric_limits<Real>::max();
			Real cur_dis(0);
			for (int i(0); i < m_found_optimums.size(); ++i) {
				cur_dis=m_found_optimums[i].variableDistance(sol, id_pro);
				if (minDis > cur_dis) {
					minDis = cur_dis;
					idx = i;
				}
			}
		}

		Real getDeratingFactor(Real dis, Real ref) {
			if (ref <= 0) return 0;
			double f = dis * dis*dis / (ref*ref*ref);
			f = exp(-f)*(1 - f);
			return f > 0 ? f : 0;;
		}


		bool derateFitness(Solution<> & x, int id_pro, int id_alg, int id_rnd) {
			if (m_found_optimums.empty()) return false;
			int idx(0);
			Real dis(0);
			get_nearest_optimum(x, idx, dis, id_pro);
			if (idx == -1)return false;
			double refdis =m_found_optimums[idx].getRefRadi(x, id_pro, id_alg, id_rnd);
			if (dis < refdis) {		
				x.objective(0) = m_found_optimums[idx].getDeratingObj(id_pro, id_alg);
				return true;
			}
			return false;
		}
	};



}


#endif