#include "custom_method.h"
#include "../core/global.h"
//#include "../instance/record/rcr_vec_real.h"
//#include "../instance/record/dynamic/rcr_vec_real_dynamic.h"
#include "../utility/factory.h"

namespace ofec {
	Record* getNewRecord(const ParamMap& param) {
		Record* record;
		auto alg_tag = Factory<Algorithm>::get().at(std::get<std::string>(param.at("algorithm name"))).second;
		//if (alg_tag.find(ProTag::kDOP) != alg_tag.end())
		//	record = new RecordVecRealDynamic(param);
		//else record = new RecordVecReal(param);
		return record;
	}
}