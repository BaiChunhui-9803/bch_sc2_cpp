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

float sc2::RunBot::simulate_single_solution_back_score(const Solution& s) {
	//m_bot_allocation.LaunchMultiGame(m_population_size, m_simulate_step_size);
	//m_bot_allocation
	return 0.0f;
}
