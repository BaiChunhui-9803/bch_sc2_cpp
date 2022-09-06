#ifndef OFEC_RECORD_VEC_REAL_DYNAMIC_H
#define OFEC_RECORD_VEC_REAL_DYNAMIC_H

#include "../../../core/record/record.h"
#include <string>
#include <vector>

namespace ofec {
	class RecordVecRealDynamic : public Record {
	protected:
		std::map<int, std::vector<Real>> m_data;
		std::vector<std::string> m_heading;
	public:
		RecordVecRealDynamic(const ParamMap& param) : Record(param) {}
		void outputData(const std::list<int>& ids_pros, const std::list<int>& ids_algs) override;
		virtual void record(int id_alg, const std::vector<Real>& vals);
	protected:
		void outputProgress();
		void outputFinal();
	};
}

#endif // !OFEC_RECORD_VEC_REAL_DYNAMIC_H
