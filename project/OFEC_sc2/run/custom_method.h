#ifndef OFEC_USER_PREF_H
#define OFEC_USER_PREF_H

#include "../core/record/record.h"
#include <memory>

namespace ofec {
	void run();
	Record* getNewRecord(const ParamMap& param);
	void registerParamAbbr();
	void registerProblem();
	void registerAlgorithm();
	void customizeFileName();
}

#endif // !OFEC_USER_PREF_H


