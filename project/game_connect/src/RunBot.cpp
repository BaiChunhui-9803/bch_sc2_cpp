#include "RunBot.h"

Solution sc2::RunBot::run(std::vector<Command> load_commands, State load_state) {
	int POPSIZE = 1;
	int STEPSIZE = 100;
	m_bot_allocation.LaunchMultiGame(POPSIZE, STEPSIZE, load_commands);
	std::vector<std::pair<size_t, MyScore>> scorer = m_bot_allocation.getScorer();
	for (int i = 0; i < scorer.size(); ++i) {
		std::cout << "scorer[" << scorer.at(i).first << "]\t"
			<< "damage_to_enemy:" << scorer.at(i).second.m_damage_to_enemy << "\t"
			<< "damage_to_self:" << scorer.at(i).second.m_damage_to_self << "\t"
			<< "total_score:" << scorer.at(i).second.m_total_score << std::endl;
	}

	return Solution();
}

std::vector<std::pair<size_t, MyScore>> sc2::RunBot::runSingleSolution(std::vector<Command> load_commands, State load_state) {
	// 每个simulator占用一个Command
	if (load_state.isBlank()) {
		m_bot_allocation.LaunchMultiGame(m_population_size, m_simulate_step_size, load_commands);
	}
	else {
		m_bot_allocation.LaunchMultiGame(m_population_size, m_simulate_step_size, load_commands, load_state);
	}
	std::vector<std::pair<size_t, MyScore>> scorer = m_bot_allocation.getScorer();
	return scorer;
}

Solution sc2::RunBot::generateSolution(State load_state) {
	Solution sol(2 * command_length);
	sol.s_commands.c_actions.resize(2 * command_length);
	float trained_weight = 40.0;
	if (load_state.isBlank()) {
		for (size_t i = 0; i < 2 * command_length; ++i) {
			if (i % 2 == 0) {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::SMART;
				sol.s_commands.c_actions[i].target_type = ActionRaw::TargetType::TargetPosition;
				sol.s_commands.c_actions[i].target_point = Point2D(55.8493, 64.4088) + Point2DInPolar(GetRandomFraction() * trained_weight, GetRandomFraction() * 2 * PI).toPoint2D();
			}
			else {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::ATTACK;
				sol.s_commands.c_actions[i].target_type = ActionRaw::TargetType::TargetPosition;
				sol.s_commands.c_actions[i].target_point = Point2D(55.8493, 64.4088) + Point2DInPolar(GetRandomFraction() * trained_weight, GetRandomFraction() * 2 * PI).toPoint2D();
			}
		}
	}
	else {
		for (size_t i = 0; i < 2 * command_length; ++i) {
			if (i % 2 == 0) {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::SMART;
				sol.s_commands.c_actions[i].target_type = ActionRaw::TargetType::TargetPosition;
				sol.s_commands.c_actions[i].target_point = load_state.getCenterPos() + Point2DInPolar(GetRandomFraction() * trained_weight, GetRandomFraction() * 2 * PI).toPoint2D();
			}
			else {
				sol.s_commands.c_actions[i].ability_id = ABILITY_ID::ATTACK;
				sol.s_commands.c_actions[i].target_type = ActionRaw::TargetType::TargetPosition;
				sol.s_commands.c_actions[i].target_point = load_state.getCenterPos() + Point2DInPolar(GetRandomFraction() * trained_weight, GetRandomFraction() * 2 * PI).toPoint2D();
			}
		}
	}

	return sol;
}










float sc2::RunBot::simulate_single_solution_back_score(const Solution& s) {
	//m_bot_allocation.LaunchMultiGame(m_population_size, m_simulate_step_size);
	//m_bot_allocation
	return 0.0f;
}
