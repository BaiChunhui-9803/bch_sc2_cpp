#ifndef RUNBOT_H
#define RUNBOT_H

#include <iostream>
#include <sc2api/sc2_api.h>
#include <map>
#include <functional>
#include "bot_allocation.h"
#include "Solution.h"

using namespace std::placeholders;

namespace sc2 {

	class RunBot :public Agent {
		using Population = std::vector<Solution>;
		using Evaluator = std::function<float(const Solution& s)>;

	private:
		// 一个bot分配器，在这里模拟运行多个bot并返回解的vector
		BotAllocation m_bot_allocation;

		const double PI = atan(1.) * 4.;
		// Some data from the proto is presented in old 'normal' speed, now I should reset them to the faster speed.
		const float m_frames_per_second = 16;

		// game data should be updated
		Units m_alive_self_units;
		Units m_alive_enemy_units;
		Units m_all_alive_units;
		bool m_is_save = false; // for forward model
	   // constant game data
		GameInfo m_game_info;
		UnitTypes m_unit_types;
		UnitTypeData m_marine; // for convenient

		// algorithm configuration
		const int command_length = 1;
		const float zero_potential_energy_ratio = 0.8f;
		const float advantage_range_factor = 3.f; // how long the attractive field from enemies can reach
		const double m_attack_prob = 1;
		const double m_killing_bonus_factor = 3;
		//std::function<bool(const Solution&, const Solution&)> m_compare_function = multi_greater;

		int m_objective_number;
		const int m_population_size = 10;
		const int m_simulate_step_size = 100;
		const int m_offspring_size = 10;
		const float m_muatation_rate = 0.1f;
		const float m_crossover_rate = 1;
		const double m_theta_mutate_step = 2 * PI / 10.;
		const int m_produce_times = 20;

		// algorithm content
		Population m_population;
		std::vector<float> m_damage_objective;
		std::vector<float> m_threat_objective;
		std::vector<Evaluator> m_evaluators;

	public:
		//virtual void OnGameStart() final;
		//virtual void OnStep() final;
		//todo control units by rules
		//virtual void OnUnitIdle(const Unit* u) final;
		//virtual void OnGameEnd() final;

		RunBot() {
			m_population.reserve(m_population_size + m_offspring_size);
			m_damage_objective.reserve(m_population_size + m_offspring_size);
			m_threat_objective.reserve(m_population_size + m_offspring_size);
			// 目前只考虑单个适应值
			m_objective_number = 1;
			m_evaluators.resize(m_objective_number);
			m_evaluators[0] = std::bind(&RunBot::simulate_single_solution_back_score, this, _1);
		}

		Solution run(std::vector<Command> load_commands = std::vector<Command>(), State load_state = State());

	private:

		// generate a random solution
		//Solution generate_random_solution();


		//void evaluate_solutions(Population& p);


		// 算子部分
		float simulate_single_solution_back_score(const Solution& s);
	};

}

#endif