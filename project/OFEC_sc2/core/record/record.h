#ifndef OFEC_RECORD_H
#define OFEC_RECORD_H

#include <sstream>
#include <list>
#include "../../utility/typevar/typevar.h"

namespace ofec {
	class Record {
	protected:
		ParamMap m_params;
		std::stringstream m_filename;
	public:
		virtual void outputData(const std::list<int> &ids_pros, const std::list<int> &ids_algs) = 0;
		virtual ~Record() {}
	protected:
		Record(const ParamMap &param);
		void setFileName();
	};
}

#endif // !OFEC_RECORD_H
