#ifndef RUNBOT_H
#define RUNBOT_H

#define PopSize 2                  //初始种群大小
#define GenerationsMaxN 200         //最大代数
#define CommandSize 10				//一个解的命令长度
#define SimulateStepSize 100		//模拟步长
#define CountN 10                   //控制代数
#define p_cross 0.7                 //交叉概率
#define p_mutate 0.05               //变异概率


#include <iostream>
#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_arg_parser.h"
#include "sc2utils/sc2_manage_process.h"
#include <map>
#include <functional>
#include "bot_allocation.h"
#include "Solution.h"

enum Game_Stage {
	//主客户端运行环节
	Update_Flag = 0,
	//辅客户端模拟环节
	Simulate_Flag = 1
};

using namespace std::placeholders;

namespace sc2 {

	class RunBot :public Agent {
		using Population = std::vector<Solution>;
		using Evaluator = std::function<float(const Solution& s)>;

	public:
		// 游戏启动执行
		virtual void OnGameStart() final;
		// 游戏走帧执行 Your bots OnStep function will be called each time the coordinator steps the simulation forward.
		virtual void OnStep() final;
		// 单位被摧毁执行
		virtual void OnUnitDestroyed(const Unit* unit) override;
		virtual void OnUnitIdle(const sc2::Unit* unit_) override;
	private:
		// 一个bot分配器，在这里模拟运行多个bot并返回解的vector
		BotAllocation m_bot_allocation;

		const double PI = atan(1.) * 4.;

		Population m_population;

		Solution m_best_solution;

	public:
		size_t step = 50;
		State m_save_state = State();
		Game_Stage m_game_stage = Update_Flag;
		//virtual void OnGameStart() final;
		//virtual void OnStep() final;
		//todo control units by rules
		//virtual void OnUnitIdle(const Unit* u) final;
		//virtual void OnGameEnd() final;

		RunBot() {
			m_population.reserve(PopSize);

			//m_evaluators[0] = std::bind(&RunBot::simulate_single_solution_back_score, this, _1);
		}

		std::vector<std::pair<size_t, MyScore>> runMultiSolution(std::vector<Solution> load_solutions, State load_state = State());

		Solution generateSolution(State load_state = State());

		std::vector<Solution> GA();

		void initializeAlgorithm();

		void initializePopulation();

		void calculateFitness();

	private:


		// generate a random solution
		//Solution generate_random_solution();


		//void evaluate_solutions(Population& p);


		// 算子部分
		float simulate_single_solution_back_score(const Solution& s);
	};

}

#endif