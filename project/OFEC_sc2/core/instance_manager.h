#ifndef OFEC_INSTANCE_MANAGER_H
#define OFEC_INSTANCE_MANAGER_H

#include <memory>
#include <vector>
#include <shared_mutex>

#include "record/record.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"
#include "../utility/random/newran.h"

namespace ofec {
	enum class MngrTag {
		Success,
		ProblemOccupied,
		RecordOccupied,
		RandomOccupied,
		RandomDoNoExist,
		ProblemDoNoExist,
		AlgorithmDoNotExist,
		RecordDoNoExist,
		ParamMapDoNoExist
	};

	class InstanceManager {
	private:
		std::vector<std::unique_ptr<Random>> m_randoms;
		std::vector<std::unique_ptr<Problem>> m_problems;
		std::vector<std::unique_ptr<Algorithm>> m_algorithms;
		std::vector<std::unique_ptr<ParamMap>> m_param_maps;
		std::vector<std::unique_ptr<Record>> m_records;

		std::vector<size_t> m_num_occupied_rnd;
		std::vector<size_t> m_num_occupied_pro;
		std::vector<size_t> m_num_occupied_alg;
		std::vector<size_t> m_num_occupied_param;
		std::vector<size_t> m_num_occupied_rcr;

		std::list<size_t> m_shelved_ids_pro;
		std::list<size_t> m_shelved_ids_alg;
		std::list<size_t> m_shelved_ids_param;
		std::list<size_t> m_shelved_ids_rnd;
		std::list<size_t> m_shelved_ids_rcr;

		mutable std::shared_mutex m_pro_mtx, m_alg_mtx, m_param_mtx, m_rnd_mtx, m_rcr_mtx;
		
	public:
		static std::unique_ptr<InstanceManager> ms_instance_manager;

		int addRandom(Real seed);
		int addRecord(int id_param);
		int addProblem(int id_param, Real seed);
		int addProblem(int id_param, std::unique_ptr<Problem> &pro, Real seed);
		int addAlgorithm(int id_param, int id_pro, Real seed, int id_rcd = -1);
		int addAlgorithm(int id_param, std::unique_ptr<Algorithm> &alg, int id_pro, Real seed, int id_rcd = -1);
		int addParamaps(const ParamMap &param);
		int addParamaps(ParamMap &&param);

		Random &getRandom(int id) const;
		Record &getRecord(int id) const;
		Problem &getProblem(int id) const;
		Algorithm &getAlgorithm(int id) const;
		const ParamMap &getParamMap(int id) const;

		MngrTag removeRecord(int id_rcr);
		MngrTag removeProblem(int id_pro);
		MngrTag removeAlgorithm(int id_alg);
		MngrTag removeRandom(int id_rnd);
		MngrTag removeParamMap(int id_param);

		bool isRandomIdValid(int id) const;
		bool isRecordIdValid(int id) const;
		bool isProblemIdValid(int id) const;
		bool isAlgorithmIdValid(int id) const;
		bool isParamMapIdValid(int id) const;
	};

#define ADD_RND(seed) InstanceManager::ms_instance_manager->addRandom(seed)
#define ADD_RCR(id_param) InstanceManager::ms_instance_manager->addRecord(id_param)
#define ADD_PRO(id_param, seed) InstanceManager::ms_instance_manager->addProblem(id_param, seed)
#define ADD_PRO_EXST(id_param, pro, seed) InstanceManager::ms_instance_manager->addProblem(id_param, pro, seed)
#define ADD_ALG(id_param, id_pro, seed, id_rcr) InstanceManager::ms_instance_manager->addAlgorithm(id_param, id_pro, seed, id_rcr)
#define ADD_ALG_EXST(id_param, alg, id_pro, seed, id_rcr) InstanceManager::ms_instance_manager->addAlgorithm(id_param, alg, id_pro, seed, id_rcr)
#define ADD_PARAM(param) InstanceManager::ms_instance_manager->addParamaps(param)

#define GET_RND(id_rnd) InstanceManager::ms_instance_manager->getRandom(id_rnd)
#define GET_RCR(id_rcr) InstanceManager::ms_instance_manager->getRecord(id_rcr)
#define GET_PRO(id_pro) InstanceManager::ms_instance_manager->getProblem(id_pro)
#define GET_ALG(id_alg) InstanceManager::ms_instance_manager->getAlgorithm(id_alg)
#define GET_PARAM(id_param) InstanceManager::ms_instance_manager->getParamMap(id_param)

#define DEL_RND(id_rnd) InstanceManager::ms_instance_manager->removeRandom(id_rnd)
#define DEL_RCR(id_rcr) InstanceManager::ms_instance_manager->removeRecord(id_rcr)
#define DEL_PRO(id_pro) InstanceManager::ms_instance_manager->removeProblem(id_pro)
#define DEL_ALG(id_alg) InstanceManager::ms_instance_manager->removeAlgorithm(id_alg)
#define DEL_PARAM(id_param) InstanceManager::ms_instance_manager->removeParamMap(id_param)

#define ID_RND_VALID(id_rnd) InstanceManager::ms_instance_manager->isRandomIdValid(id_rnd)
#define ID_RCR_VALID(id_rcr) InstanceManager::ms_instance_manager->isRecordIdValid(id_rcr)
#define ID_PRO_VALID(id_pro) InstanceManager::ms_instance_manager->isProblemIdValid(id_pro)
#define ID_ALG_VALID(id_alg) InstanceManager::ms_instance_manager->isAlgorithmIdValid(id_alg)
#define ID_PARAM_VALID(id_param) InstanceManager::ms_instance_manager->isParamMapIdValid(id_param)
}

#endif // !OFEC_INSTANCE_MANAGER_H
