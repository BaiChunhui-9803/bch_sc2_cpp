#include "sp_operator.h"
#include"../../../../problem/combination/selection_problem/selection_problem.h"


int OFEC::SelectionProblemOperator::learn_from_local(const IndividualType& cur, int id_rnd, int id_pro, const TInterpreter& interpreter) const
{
	if (interpreter.stepFinish(id_pro, cur)) {
		return -1;
	}
	else {
		return cur.variable()[cur.getLoc()];
	}
}

int OFEC::SelectionProblemOperator::learn_from_global(const IndividualType& cur, int id_rnd, int id_pro, const TInterpreter& interpreter, const std::function<Real(const IndividualType& cur, int to)>& proFun) const
{
	if (interpreter.stepFinish(id_pro, cur)) {
		return -1;
	}	
	int matSize=interpreter.getMatrixSize()[interpreter.curPositionIdx(id_pro,cur)];
	std::function<Real(int to)> nei_weight_fun = [&](int to) {
		return proFun(cur, to);
	};
	auto next_step_iter(GET_RND(id_rnd).uniform.spinWheel<int,Real>(0, matSize, nei_weight_fun));
	if (next_step_iter == matSize)next_step_iter = -1;
	return next_step_iter;
}

int OFEC::SelectionProblemOperator::learn_from_other(const IndividualType& cur, int id_rnd, int id_pro, const TInterpreter& interpreter, const IndividualType& other) const
{
	if (interpreter.stepFinish(id_pro, cur)) {
		return -1;
	}
	else {
		return other.variable()[cur.getLoc()];
	}
	
}


bool OFEC::SelectionProblemOperator::learn_from_other(IndividualType& cur, int id_rnd, int id_pro, const InterpreterType& interpreter, const IndividualType& other, Real radius) const
{
	Real dis(cur.variableDistance(other, id_pro));
	
	if (dis > radius) {
		cur.reset();
		int numVar = std::ceil((dis - radius) * cur.variable().size());
		std::vector<int> idxs(cur.variable().size());
		for (int idx(0); idx < cur.variable().size();++idx) {
			idxs[idx] = idx;
		}
		GET_RND(id_rnd).uniform.shuffle(idxs.begin(), idxs.end());
		for (auto& dim : idxs) {
			if (cur.variable()[dim] != other.variable()[dim]) {
				cur.variable()[dim] = other.variable()[dim];
				if (dim + 1 < cur.variable().size()) {
					cur.variable()[dim+1] = other.variable()[dim+1];
					--numVar;
					if (numVar == 0)break;
				}

			}
		}
		return true;
	}
	return false;
}

void OFEC::SelectionProblemOperator::localSearch(IndividualType& cur, int id_rnd, int id_pro, int id_alg, int totalEvals, int curType)
{

//	std::cout << "localSearch" << std::endl;
	thread_local std::vector<std::pair<int, int>> neis;
	IndividualType neiIndividual(cur);
	neiIndividual.reset();
	neis.clear();
	auto& map(GET_DYN_SP(id_pro).get_map());
	for (int dimIdx(0); dimIdx < map.m_wall_types.size(); ++dimIdx) {
		for (int posIdx(0); posIdx < map.m_stations[dimIdx].size(); ++posIdx) {
			neis.push_back(std::make_pair(dimIdx, posIdx));
		}
	}
	GET_RND(id_rnd).uniform.shuffle(neis.begin(), neis.end());
	int neiIdx(0);
	while (totalEvals--) {

		auto& nei(neis[neiIdx]);
		neiIndividual.variable()[nei.first] = nei.second;
		neiIndividual.reset();
		evaluate(id_pro, id_alg, id_rnd, neiIndividual, true);
		if (better(id_pro, neiIndividual, cur)) {
			cur = neiIndividual;
		}
		else neiIndividual = cur;
		while (neiIndividual.variable()[neis[neiIdx].first] != neis[neiIdx].second) {
			++neiIdx;
			if (neiIdx == neis.size()) {
				GET_RND(id_rnd).uniform.shuffle(neis.begin(), neis.end());
				neiIdx = 0;
			}
		}
	}
}




int OFEC::SelectionProblemOperator::evaluate(int id_pro, int id_alg, int id_rnd, IndividualType& curSol, bool effective) const
{
	std::vector<Real> origin(curSol.objective());

	int rf = 0;
	
	switch (m_fitness_type) {
	case fitnessCalType::Objective: {
		int rf = curSol.evaluate(id_pro, id_alg, effective);
		break;
	}
	case fitnessCalType::HLS: {

		std::vector< std::unique_ptr<SolBase>> samples(m_sample_num);
		GET_DYN_SP(id_pro).generate_HLS_samples<SP_individual>(curSol, samples, id_rnd);
		//generateSamplesSolutions(id_pro, id_rnd, curSol, samples);
		Real fitness(0);
		for (auto& it : samples) {
			rf |= it->evaluate(id_pro, id_alg, effective);
			fitness += it->objective()[0];
		}
		rf |= curSol.evaluate(id_pro, id_alg, effective);
		fitness += curSol.objective()[0];
		fitness /= (m_sample_num + 1.0);
		curSol.objective()[0] = fitness;

		break;
	}
	}

	//if (GET_PRO(id_pro).hasTag(ProTag::NoisyOP)) {
	//	int curTimes = ++curSol.evaluateTimes();
	//	curSol.objective()[0] = (curSol.objective()[0] + origin.front() * (curTimes-1))/Real(curTimes);
	//}



	return rf;
}
