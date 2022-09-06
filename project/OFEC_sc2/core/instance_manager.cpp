#include "instance_manager.h"
#include "../run/custom_method.h"
#include "../utility/factory.h"

namespace ofec {
	std::unique_ptr<InstanceManager> InstanceManager::ms_instance_manager = nullptr;

	int InstanceManager::addRecord(int id_param) {
		std::unique_lock<std::shared_mutex> lock(m_rcr_mtx);
		int id_rcr(0);
		if (m_shelved_ids_rcr.empty()) {
			id_rcr = m_records.size();
			m_records.emplace_back(getNewRecord(getParamMap(id_param)));
			m_num_occupied_rcr.emplace_back(0);
		}
		else {
			id_rcr = m_shelved_ids_rcr.front();
			m_shelved_ids_rcr.pop_front();
			m_records[id_rcr].reset(getNewRecord(getParamMap(id_param)));
		}
		return id_rcr;
	}

	int InstanceManager::addProblem(int id_param, Real seed) {
		if (!isParamMapIdValid(id_param))
			throw MyExcept("Invalid ParamMap ID.");
		else {
			std::unique_lock<std::shared_mutex> lock(m_pro_mtx);
			int id_pro(0);
			if (m_shelved_ids_pro.empty()) {
				id_pro = m_problems.size();
				m_problems.emplace_back(Factory<Problem>::produce(std::get<std::string>(getParamMap(id_param).at("problem name"))));
				m_num_occupied_pro.emplace_back(0);
			}
			else {
				id_pro = m_shelved_ids_pro.front();
				m_shelved_ids_pro.pop_front();
				m_problems[id_pro].reset(Factory<Problem>::produce(std::get<std::string>(getParamMap(id_param).at("problem name"))));
			}
			int id_rnd = addRandom(seed);
			m_problems[id_pro]->m_id_pro = id_pro;
			m_problems[id_pro]->m_id_rnd = id_rnd;
			m_problems[id_pro]->m_id_param = id_param;
			++m_num_occupied_rnd[id_rnd];
			++m_num_occupied_param[id_param];
			return id_pro;
		}
	}

	int InstanceManager::addProblem(int id_param, std::unique_ptr<Problem> &pro, Real seed) {
		if (!isParamMapIdValid(id_param))
			throw MyExcept("Invalid ParamMap ID.");
		else {
			std::unique_lock<std::shared_mutex> lock(m_pro_mtx);
			int id_pro(0);
			if (m_shelved_ids_pro.empty()) {
				id_pro = m_problems.size();
				m_problems.emplace_back(pro.release());
				m_num_occupied_pro.emplace_back(0);
			}
			else {
				id_pro = m_shelved_ids_pro.front();
				m_shelved_ids_pro.pop_front();
				m_problems[id_pro].reset(pro.release());
			}
			int id_rnd = addRandom(seed);
			m_problems[id_pro]->m_id_pro = id_pro;
			m_problems[id_pro]->m_id_rnd = id_rnd;
			m_problems[id_pro]->m_id_param = id_param;
			++m_num_occupied_rnd[id_rnd];
			++m_num_occupied_param[id_param];
			return id_pro;
		}
	}

	int InstanceManager::addAlgorithm(int id_param, int id_pro, Real seed, int id_rcr) {
		if (!isParamMapIdValid(id_param))
			throw MyExcept("Invalid ParamMap ID.");
		else if (!isProblemIdValid(id_pro))
			throw MyExcept("Invalid Problem ID.");
		else {
			std::unique_lock<std::shared_mutex> lock(m_alg_mtx);
			int id_alg(0);
			if (m_shelved_ids_alg.empty()) {
				id_alg = m_algorithms.size();
				m_algorithms.emplace_back(Factory<Algorithm>::produce(std::get<std::string>(getParamMap(id_param).at("algorithm name"))));
				m_num_occupied_alg.emplace_back(0);
			}
			else {
				id_alg = m_shelved_ids_alg.front();
				m_shelved_ids_alg.pop_front();
				m_algorithms[id_alg].reset(Factory<Algorithm>::produce(std::get<std::string>(getParamMap(id_param).at("algorithm name"))));
			}
			int id_rnd = addRandom(seed);
			m_algorithms[id_alg]->m_id_param = id_param;
			m_algorithms[id_alg]->m_id_rnd = id_rnd;
			m_algorithms[id_alg]->m_id_pro = id_pro;
			m_algorithms[id_alg]->m_id_rcr = id_rcr;
			m_algorithms[id_alg]->m_id_alg = id_alg;
			++m_num_occupied_param[id_param];
			++m_num_occupied_rnd[id_rnd];
			++m_num_occupied_pro[id_pro];
			if (isRecordIdValid(id_rcr))
				++m_num_occupied_rcr[id_rcr];
			return id_alg;
		}
	}

	int InstanceManager::addAlgorithm(int id_param, std::unique_ptr<Algorithm> &alg, int id_pro, Real seed, int id_rcr) {
		if (!isParamMapIdValid(id_param))
			throw MyExcept("Invalid ParamMap ID.");
		else if (!isProblemIdValid(id_pro))
			throw MyExcept("Invalid Problem ID.");
		else {
			std::unique_lock<std::shared_mutex> lock(m_alg_mtx);
			int id_alg(0);
			if (m_shelved_ids_alg.empty()) {
				id_alg = m_algorithms.size();
				m_algorithms.emplace_back(alg.release());
				m_num_occupied_alg.emplace_back(0);
			}
			else {
				id_alg = m_shelved_ids_alg.front();
				m_shelved_ids_alg.pop_front();
				m_algorithms[id_alg].reset(alg.release());
			}
			int id_rnd = addRandom(seed);
			m_algorithms[id_alg]->m_id_param = id_param;
			m_algorithms[id_alg]->m_id_rnd = id_rnd;
			m_algorithms[id_alg]->m_id_pro = id_pro;
			m_algorithms[id_alg]->m_id_rcr = id_rcr;
			m_algorithms[id_alg]->m_id_alg = id_alg;
			++m_num_occupied_param[id_param];
			++m_num_occupied_rnd[id_rnd];
			++m_num_occupied_pro[id_pro];
			if (isRecordIdValid(id_rcr))
				++m_num_occupied_rcr[id_rcr];
			return id_alg;
		}
	}

	int InstanceManager::addParamaps(const ParamMap &param) {
		std::unique_lock<std::shared_mutex> lock(m_param_mtx);
		int id_param(0);
		if (m_shelved_ids_param.empty()) {
			id_param = m_param_maps.size();
			m_param_maps.emplace_back(new ParamMap(param));
			m_num_occupied_param.emplace_back(0);
		}
		else {
			id_param = m_shelved_ids_param.front();
			m_shelved_ids_param.pop_front();
			m_param_maps[id_param].reset(new ParamMap(param));
		}
		return id_param;
	}

	int InstanceManager::addParamaps(ParamMap &&param) {
		std::unique_lock<std::shared_mutex> lock(m_param_mtx);
		int id_param(0);
		if (m_shelved_ids_param.empty()) {
			id_param = m_param_maps.size();
			m_param_maps.emplace_back(new ParamMap(std::move(param)));
			m_num_occupied_param.emplace_back(0);
		}
		else {
			id_param = m_shelved_ids_param.front();
			m_shelved_ids_param.pop_front();
			m_param_maps[id_param].reset(new ParamMap(std::move(param)));
		}
		return id_param;
	}

	int InstanceManager::addRandom(Real seed) {
		std::unique_lock<std::shared_mutex> lock(m_rnd_mtx);
		int id_rnd;
		if (m_shelved_ids_rnd.empty()) {
			id_rnd = m_randoms.size();
			m_randoms.emplace_back(new Random(seed));
			m_num_occupied_rnd.emplace_back(0);
		}
		else {
			id_rnd = m_shelved_ids_rnd.front();
			m_shelved_ids_rnd.pop_front();
			m_randoms[id_rnd].reset(new Random(seed));
		}
		return id_rnd;
	}

	Random& InstanceManager::getRandom(int id) const {
		if (!isRandomIdValid(id))
			throw MyExcept("Invalid Random ID.");
		else {
			std::shared_lock<std::shared_mutex> lock(m_rnd_mtx);
			return *m_randoms[id];
		}
	}

	Record& InstanceManager::getRecord(int id) const {
		if (!isRecordIdValid(id))
			throw MyExcept("Invalid Record ID.");
		else {
			std::shared_lock<std::shared_mutex> lock(m_rcr_mtx);
			return *m_records[id];
		}
	}

	Problem& InstanceManager::getProblem(int id) const {
		if (!isProblemIdValid(id))
			throw MyExcept("Invalid Problem ID.");
		else {
			std::shared_lock<std::shared_mutex> lock(m_pro_mtx);
			return *m_problems[id];
		}
	}

	Algorithm& InstanceManager::getAlgorithm(int id) const {
		if (!isAlgorithmIdValid(id))
			throw MyExcept("Invalid Algorithm ID.");
		else {
			std::shared_lock<std::shared_mutex> lock(m_alg_mtx);
			return *m_algorithms[id];
		}
	}

	const ParamMap& InstanceManager::getParamMap(int id) const {
		if (!isParamMapIdValid(id))
			throw MyExcept("Invalid ParamMap ID.");
		else {
			std::shared_lock<std::shared_mutex> lock(m_param_mtx);
			return *m_param_maps[id];
		}
	}

	MngrTag InstanceManager::removeAlgorithm(int id) {
		if (!isAlgorithmIdValid(id))
			return MngrTag::AlgorithmDoNotExist;
		else {
			std::unique_lock<std::shared_mutex> lock(m_alg_mtx);
			if (isProblemIdValid(m_algorithms[id]->m_id_pro))
				--m_num_occupied_pro[m_algorithms[id]->m_id_pro];
			if (isRecordIdValid(m_algorithms[id]->m_id_rcr))
				--m_num_occupied_rcr[m_algorithms[id]->m_id_rcr];
			if (isRandomIdValid(m_algorithms[id]->m_id_rnd))
				--m_num_occupied_rnd[m_algorithms[id]->m_id_rnd];
			if (isParamMapIdValid(m_algorithms[id]->m_id_param))
				--m_num_occupied_param[m_algorithms[id]->m_id_param];
			removeRandom(m_algorithms[id]->m_id_rnd);
			m_algorithms[id].reset();
			m_shelved_ids_alg.push_back(id);
			return MngrTag::Success;
		}
	}

	MngrTag InstanceManager::removeProblem(int id) {
		if (!isProblemIdValid(id))
			return MngrTag::ProblemDoNoExist;
		else if (m_num_occupied_pro[id])
			return MngrTag::ProblemOccupied;
		else {
			std::unique_lock<std::shared_mutex> lock(m_pro_mtx);
			if (isRandomIdValid(m_problems[id]->m_id_rnd))
				--m_num_occupied_rnd[m_problems[id]->m_id_rnd];
			if (isParamMapIdValid(m_problems[id]->m_id_param))
				--m_num_occupied_param[m_problems[id]->m_id_param];
			removeRandom(m_problems[id]->m_id_rnd);
			m_problems[id].reset();
			m_shelved_ids_pro.push_back(id);
			return MngrTag::Success;
		}
	}

	MngrTag InstanceManager::removeRandom(int id) {
		if (!isRandomIdValid(id))
			return MngrTag::RandomDoNoExist;
		else if (m_num_occupied_rnd[id])
			return MngrTag::RandomOccupied;
		else {
			std::unique_lock<std::shared_mutex> lock(m_rnd_mtx);
			m_randoms[id].reset();
			m_shelved_ids_rnd.push_back(id);
			return MngrTag::Success;
		}
	}

	MngrTag InstanceManager::removeParamMap(int id) {
		if (!isParamMapIdValid(id))
			return MngrTag::RandomDoNoExist;
		else if (m_num_occupied_param[id])
			return MngrTag::RandomOccupied;
		else {
			std::unique_lock<std::shared_mutex> lock(m_param_mtx);
			m_param_maps[id].reset();
			m_shelved_ids_param.push_back(id);
			return MngrTag::Success;
		}
	}

	MngrTag InstanceManager::removeRecord(int id_rcr) {
		if (!isRecordIdValid(id_rcr))
			return MngrTag::RecordDoNoExist;
		else if (!m_num_occupied_rcr[id_rcr])
			return MngrTag::RecordOccupied;
		else {
			std::unique_lock<std::shared_mutex> lock(m_rcr_mtx);
			m_records[id_rcr].reset();
			m_shelved_ids_rcr.push_back(id_rcr);
			return MngrTag::Success;
		}
	}

	bool InstanceManager::isRandomIdValid(int id) const {
		std::shared_lock<std::shared_mutex> lock(m_rnd_mtx);
		return (id >= 0 && id < m_randoms.size() && m_randoms.at(id));
	}

	bool InstanceManager::isRecordIdValid(int id) const {
		std::shared_lock<std::shared_mutex> lock(m_rcr_mtx);
		return (id >= 0 && id < m_records.size() && m_records.at(id));
	}

	bool InstanceManager::isProblemIdValid(int id) const {
		std::shared_lock<std::shared_mutex> lock(m_pro_mtx);
		return (id >= 0 && id < m_problems.size() && m_problems.at(id));
	}

	bool InstanceManager::isAlgorithmIdValid(int id) const {
		std::shared_lock<std::shared_mutex> lock(m_alg_mtx);
		return (id >= 0 && id < m_algorithms.size() && m_algorithms.at(id));
	}

	bool InstanceManager::isParamMapIdValid(int id) const {
		std::shared_lock<std::shared_mutex> lock(m_param_mtx);
		return (id >= 0 && id < m_param_maps.size() && m_param_maps.at(id));
	}
}