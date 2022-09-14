#include "RunBot.h"

void RunBot::OnGameStart() {
	std::cout << "New game started!" << std::endl;
}

void RunBot::OnStep() {
	const ObservationInterface* observation = Observation();
	sc2::DebugInterface* debug = Debug();

	Units units_self = observation->GetUnits(Unit::Alliance::Self);
	Units units_enemy = observation->GetUnits(Unit::Alliance::Enemy);

	if (!game_stop_observe_flag_) {
		pushObservedUnits(observation);
		setGameInf(observed_units);
	}
	if (game_load_finish_flag_ && !game_set_commands_finish_flag) {
		setOrders(m_best_solution.s_commands);
		game_set_commands_finish_flag = true;
	}

	debug->DebugMoveCamera(getCenterPos(units_self));
	debug->SendDebug();

	if (step >= 50) {
		m_save_state = m_save_state.SaveState(observation);
		step = 0;
		m_game_stage = Simulate_Flag;
	}

	++step;
}

void RunBot::OnUnitDestroyed(const sc2::Unit* unit_) {
}

void RunBot::OnUnitIdle(const sc2::Unit* unit_) {
	std::cout << sc2::UnitTypeToName(unit_->unit_type) <<
		"(" << unit_->tag << ") is idle" << std::endl;
}

std::vector<std::vector<Solution>> sc2::RunBot::turnPopToSolvec(Population population) {
	std::vector<std::vector<Solution>> sol_vec;
	sol_vec.resize(PopSize);
	for (size_t i = 0; i < PopSize; ++i) {
		sol_vec[i].resize(SimulateSize);
	}
	for (size_t i = 0; i < sol_vec.size(); ++i) {
		for (size_t j = 0; j < sol_vec[i].size(); ++j) {
			sol_vec[i][j] = population[SimulateSize * i + j];
		}
	}
	return sol_vec;
}

void RunBot::setGameInf(const std::map<sc2::Tag, sc2::Unit>& observed) {
	begin_selfHP = 0.0;
	begin_enemyHP = 0.0;
	begin_selfN = 0;
	begin_enemyN = 0;
	for (std::map<sc2::Tag, sc2::Unit>::reverse_iterator iter = observed_units.rbegin(); iter != observed_units.rend(); ++iter) {
		if (iter->second.owner == 1) {
			++begin_selfN;
			begin_selfHP += iter->second.health_max;
		}
		else if (iter->second.owner == 2) {
			++begin_enemyN;
			begin_enemyHP += iter->second.health_max;
		}
	}
}

void RunBot::pushObservedUnits(const ObservationInterface*& ob) {
	sc2::Units units = Observation()->GetUnits();
	for (auto& u : units) {
		if (observed_units.find(u->tag) == observed_units.end()) {
			observed_units.insert(std::pair<sc2::Tag, sc2::Unit>(u->tag, *u));
			if (u->alliance == sc2::Unit::Alliance::Self) {
				observed_self_units.push_back(u);
			}
			else if (u->alliance == sc2::Unit::Alliance::Enemy) {
				observed_enemy_units.push_back(u);
			}
			//std::cout << u->tag << std::endl;
		}
	}
}

Point2D RunBot::getCenterPos(const Units& units) {
	Point2D center_pos;
	for (auto& u : units) {
		center_pos += Point2D(u->pos);
	}
	return center_pos / units.size();
}

sc2::Unit* RunBot::findUnit(Tag tag) {
	std::map<sc2::Tag, sc2::Unit>::iterator l_it = observed_units.find(tag);
	if (l_it == observed_units.end()) {
		return &Unit();
	}
	else {
		return &(l_it->second);
	}
}

void RunBot::setOrders(Command commands) {
	for (const ActionRaw& act : commands.c_actions) {
		switch (act.target_type) {
		case ActionRaw::TargetType::TargetNone:
			for (auto& u : observed_self_units) {
				action->UnitCommand(u, act.ability_id, true);
			}
			break;
		case ActionRaw::TargetType::TargetPosition:
			for (auto& u : observed_self_units) {
				action->UnitCommand(u, act.ability_id, act.target_point, true);
			}
			break;
		case ActionRaw::TargetType::TargetUnitTag:
			for (auto& u : observed_self_units) {
				action->UnitCommand(u, act.ability_id, findUnit(act.target_tag), true);
			}
			break;
		}
	}
}

std::vector<std::pair<size_t, std::vector<MyScore>>> sc2::RunBot::runMultiSolution(std::vector<std::vector<Solution>> load_solutions, State load_state) {
	// 每个simulator占用一个Command
	//BotAllocation m_bot_allocation;
	if (load_state.isBlank()) {
		m_bot_allocation.LaunchMultiGame(PopSize, SimulateStepSize, load_solutions);
	}
	else {
		m_bot_allocation.LaunchMultiGame(PopSize, SimulateStepSize, load_solutions, load_state);
	}
	std::vector<std::pair<size_t, std::vector<MyScore>>> scorer = m_bot_allocation.getScorer();
	return scorer;
}

Solution sc2::RunBot::generateSolution(State load_state) {
	Solution sol(2 * CommandSize);
	sol.s_commands.c_actions.resize(2 * CommandSize);
	float trained_weight = 40.0;
	if (load_state.isBlank()) {
		for (size_t i = 0; i < 2 * CommandSize; ++i) {
			if (i % 2 == 0) {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::MOVE_MOVE;
				sol.s_commands.c_actions[i].target_type = ActionRaw::TargetType::TargetPosition;
				sol.s_commands.c_actions[i].target_point = Point2D(55.8493, 64.4088) + Point2DInPolar(GetRandomFraction() * trained_weight, GetRandomFraction() * 2 * PI).toPoint2D();
			}
			else {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::ATTACK;
				sol.s_commands.c_actions[i].target_type = ActionRaw::TargetType::TargetPosition;
				sol.s_commands.c_actions[i].target_point = sol.s_commands.c_actions[i - 1].target_point;
			}
		}
	}
	else {
		for (size_t i = 0; i < 2 * CommandSize; ++i) {
			if (i % 2 == 0) {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::MOVE_MOVE;
				sol.s_commands.c_actions[i].target_type = ActionRaw::TargetType::TargetPosition;
				sol.s_commands.c_actions[i].target_point = load_state.getCenterPos() + Point2DInPolar(GetRandomFraction() * trained_weight, GetRandomFraction() * 2 * PI).toPoint2D();
			}
			else {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::ATTACK;
				sol.s_commands.c_actions[i].target_type = ActionRaw::TargetType::TargetPosition;
				sol.s_commands.c_actions[i].target_point = sol.s_commands.c_actions[i - 1].target_point;
			}
		}
	}

	return sol;
}

Solution sc2::RunBot::GA() {

	unsigned seed;
	seed = time(0);
	srand(seed);

	//初始化种群
	initializeAlgorithm();

	initializePopulation();

	calculateFitness();

	//迭代过程
	for (m_count = 0; m_count <= GenerationsMaxN; ++m_count) {
		//选择
		select();
		//交叉
		cross();
		//变异
		mutate();

		calculateFitness();
	}

	return m_best_solution;
}

void sc2::RunBot::initializeAlgorithm() {
	m_population.resize(PopSize * SimulateSize);
	m_blank_pop.resize(PopSize * SimulateSize);
	m_best_solution.s_commands.c_actions.resize(CommandSize * 2);
}

void sc2::RunBot::initializePopulation() {
	for (size_t i = 0; i < PopSize * SimulateSize; ++i) {
		m_population[i] = generateSolution(m_save_state);
	}
}

void sc2::RunBot::calculateFitness() {

	std::vector<std::pair<size_t, std::vector<MyScore>>> scores = runMultiSolution(turnPopToSolvec(m_population), m_save_state);
	float this_best = 0.0;
	size_t this_best_id = 0;
	for (size_t i = 0; i < scores.size(); ++i) {
		for (size_t j = 0; j < scores[i].second.size(); ++j) {
			m_population[i * SimulateSize + j].s_objectives = scores[i].second[j].m_total_score;
			std::cout << "[" << i << "][" << j << "]分数:" << scores[i].second[j].m_total_score << std::endl;
			if (scores[i].second[j].m_total_score > this_best) {
				this_best = scores[i].second[j].m_total_score;
				this_best_id = i * SimulateSize + j;
			}
		}
	}

	if (this_best > m_best_solution.s_objectives) {
		m_best_solution = m_population[this_best_id];
	}

}

void sc2::RunBot::select() {
	std::vector<Solution> tempColony = m_blank_pop;
	size_t i;
	float sum = 0.0;
	float avg = 0.0;
	float s;
	float fitness[PopSize * SimulateSize] = { 0 };
	float gailv[PopSize * SimulateSize] = { 0 };
	float SelectP[PopSize * SimulateSize + 1] = { 0 };
	for (i = 0; i < m_population.size(); ++i) {
		fitness[i] = m_population[i].s_objectives;
		if (fitness[i] > 0) {
			sum += fitness[i];
		}
		else {
			gailv[i] = 2;
		}
	}
	for (i = 0; i < m_population.size(); ++i) {
		if (gailv[i] >= 1) {
			gailv[i] = 0;
		}
		else {
			gailv[i] = fitness[i] / sum;
		}
	}
	for (i = 0; i < m_population.size(); ++i) {
		SelectP[i + 1] = SelectP[i] + gailv[i] * RAND_MAX;
	}
	tempColony[0] = m_best_solution;
	for (size_t t = 1; t < m_population.size(); ++t) {
		float ran = rand() % RAND_MAX + 1;
		s = (float)ran / 100.0;
		for (i = 1; i < m_population.size(); ++i) {
			if (SelectP[i] >= s)
				break;
		}
		tempColony[t] = m_population[i - 1];
		//memcpy(tempColony[t], m_population[i - 1], sizeof(tempColony[t]))
	}
	for (i = 0; i < m_population.size(); ++i) {
		m_population[i] = tempColony[i];
		std::cout << m_population[i].s_commands.c_actions[i].target_point.x << std::endl;
	}
}

void sc2::RunBot::cross() {
	int i, j, t, l;
	int a, b, ca, cb;
	Command cmd1, cmd2;
	cmd1.c_actions.resize(2 * CommandSize);
	cmd2.c_actions.resize(2 * CommandSize);
	for (i = 0; i < m_population.size(); i++) {
		double s = ((double)(rand() % RAND_MAX)) / RAND_MAX;
		if (s < p_cross) {
			cb = rand() % m_population.size();
			ca = rand() % m_population.size();
			//if (m_population[ca] == m_best_solution || m_population[ca] == m_best_solution) {
			//	continue;
			//}
			do {
				l = rand() % CommandSize;
				a = rand() % CommandSize;
			} while (l >= a);

			for (j = 0; j < 2 * CommandSize; ++j) {
				if (j < 2 * l) {
					cmd1.c_actions[j] = m_population[ca].s_commands.c_actions[j];
				} else if (j >= 2 * l && j < 2 * a) {
					cmd1.c_actions[j] = m_population[cb].s_commands.c_actions[j];
				} else {
					cmd1.c_actions[j] = m_population[ca].s_commands.c_actions[j];
				}
			}
			m_population[i].s_objectives = 0.0;
			m_population[i].s_commands = cmd1;
		}
	}
	for (i = 0; i < m_population.size(); i++) {
		for (j = 0; j < m_population[i].s_commands.c_actions.size(); ++j) {
			if (j % 2) {
				if (m_population[i].s_commands.c_actions[j].target_point != m_population[i].s_commands.c_actions[j - 1].target_point) {
					std::cout << "不一致！" << std::endl;
					m_population[i].s_commands.c_actions[j].target_point = m_population[i].s_commands.c_actions[j - 1].target_point;
				}
			}
		}
	}
}

void sc2::RunBot::mutate() {
}










float sc2::RunBot::simulate_single_solution_back_score(const Solution& s) {
	//m_bot_allocation.LaunchMultiGame(m_population_size, m_simulate_step_size);
	//m_bot_allocation
	return 0.0f;
}
