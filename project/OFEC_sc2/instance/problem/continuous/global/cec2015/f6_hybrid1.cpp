#include "F6_hybrid1.h"
#include "../../global/classical/modified_schwefel.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/elliptic.h"

namespace ofec {
	namespace CEC2015 {
		void F6_hybrid1::setFunction() {
			m_num_function = 3;
			m_function.resize(m_num_function);
			m_param_ids.clear();
			m_start.resize(m_num_function);
			m_dim.resize(m_num_function);			
			m_hybrid_translation.resize(m_num_function);
			m_temp_sol.clear();

			size_t i, tmp;
			Real f_p[3] = { 0.3,0.3,0.4 };
			basic_func f(3);
			f[0] = &create_function<ModifiedSchwefel>;
			f[1] = &create_function<Rastrigin>;
			f[2] = &create_function<Elliptic>;

			tmp = 0;
			for (i = 0; i < m_num_function - 1; ++i) {
				m_dim[i] = (size_t)ceil(f_p[i] * m_num_vars);
				tmp += m_dim[i];
			}
			m_dim[m_num_function - 1] = m_num_vars - tmp;
			m_start[0] = 0;
			for (i = 1; i < m_num_function; ++i)
				m_start[i] = m_start[i - 1] + m_dim[i - 1];
			for (i = 0; i < m_num_function; ++i)
				m_temp_sol.push_back(Solution<>(1, 0, m_dim[i]));
			for (size_t i = 0; i < m_num_function; ++i) {	
				auto param = GET_PARAM(m_id_param);
				param["problem name"] = m_name + "_part" + std::to_string(i + 1);
				param["number of variables"] = (int)m_dim[i];
				m_param_ids[i] = ADD_PARAM(param);
				m_function[i] = dynamic_cast<Function *>(f[i]());
				m_function[i]->setIdRnd(m_id_rnd);
				m_function[i]->setIdParam(m_param_ids[i]);
				m_function[i]->initialize();
				m_function[i]->setBias(0);
			}
		}

		void F6_hybrid1::evaluateObjective(Real *x, std::vector<Real> &obj) {
			Hybrid::evaluateObjective(x, obj);
			obj[0] += 600;
		}
	}
}