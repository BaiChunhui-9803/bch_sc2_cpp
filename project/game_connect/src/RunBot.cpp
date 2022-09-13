#include "RunBot.h"

void RunBot::OnGameStart() {
	std::cout << "New game started!" << std::endl;
}

void RunBot::OnStep() {
	const ObservationInterface* observation = Observation();
	ActionInterface* action = Actions();
	sc2::DebugInterface* debug = Debug();

	Units units_self = observation->GetUnits(Unit::Alliance::Self);
	Units units_enemy = observation->GetUnits(Unit::Alliance::Enemy);

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

std::vector<std::pair<size_t, MyScore>> sc2::RunBot::runMultiSolution(std::vector<Solution> load_solutions, State load_state) {
	// 每个simulator占用一个Command
	if (load_state.isBlank()) {
		m_bot_allocation.LaunchMultiGame(PopSize, SimulateStepSize, load_solutions);
	}
	else {
		m_bot_allocation.LaunchMultiGame(PopSize, SimulateStepSize, load_solutions, load_state);
	}
	std::vector<std::pair<size_t, MyScore>> scorer = m_bot_allocation.getScorer();
	return scorer;
}

Solution sc2::RunBot::generateSolution(State load_state) {
	Solution sol(2 * CommandSize);
	sol.s_commands.c_actions.resize(2 * CommandSize);
	float trained_weight = 40.0;
	if (load_state.isBlank()) {
		for (size_t i = 0; i < 2 * CommandSize; ++i) {
			if (i % 2 == 0) {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::SMART;
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
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::SMART;
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

std::vector<Solution> sc2::RunBot::GA() {

	unsigned seed;
	seed = time(0);
	srand(seed);

	//初始化种群
	initializeAlgorithm();

	initializePopulation();

	calculateFitness();

	return std::vector<Solution>();
}

void sc2::RunBot::initializeAlgorithm() {
	m_population.resize(PopSize);
}

void sc2::RunBot::initializePopulation() {
	for (size_t i = 0; i < PopSize; ++i) {
		m_population[i] = generateSolution(m_save_state);
	}
}

void sc2::RunBot::calculateFitness() {

	std::vector<std::pair<size_t, MyScore>> scores = runMultiSolution(m_population, m_save_state);
	float this_best = 0.0;
	size_t this_best_id = 0;
	for (size_t i = 0; i < scores.size(); ++i) {
		std::cout << "id:" << i << "\tscore:" << scores[i].second.m_total_score << std::endl;
		if (scores[i].second.m_total_score > this_best) {
			this_best = scores[i].second.m_total_score;
			this_best_id = scores[i].first;

		}
	}

	if (this_best > m_best_solution.s_objectives[0]) {
		m_best_solution = m_population[this_best_id];
	}

}










float sc2::RunBot::simulate_single_solution_back_score(const Solution& s) {
	//m_bot_allocation.LaunchMultiGame(m_population_size, m_simulate_step_size);
	//m_bot_allocation
	return 0.0f;
}
