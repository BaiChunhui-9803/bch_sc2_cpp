#ifndef RUNBOT_H
#define RUNBOT_H

#define PopSize 1					//初始种群大小
#define SimulateSize 10				//一个客户端模拟解的个数
#define GenerationsMaxN 200         //最大代数
#define CommandSize 10				//一个解的命令长度
#define SimulateStepSize 100		//模拟步长
#define CountN 10                   //控制代数
#define p_cross 0.7                 //交叉概率
#define p_mutate 0.05               //变异概率


#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <random>
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
	Simulate_Flag = 1,
	//空环节
	Blank_Flag = 2
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
		std::map<sc2::Tag, sc2::Unit> observed_units;
		Units observed_self_units;
		Units observed_enemy_units;
		ActionInterface* action = Actions();
		// 一个bot分配器，在这里模拟运行多个bot并返回解的vector
		BotAllocation m_bot_allocation;
		const double PI = atan(1.) * 4.;

		Population m_population;
		Population m_blank_pop;

		Solution m_best_solution;

	public:
		int m_count = 0;
		size_t step = 50;
		State m_save_state = State();
		size_t begin_selfN = 0;
		size_t begin_enemyN = 0;
		double begin_selfHP = 0.0;
		double begin_enemyHP = 0.0;
		Game_Stage m_game_stage = Update_Flag;
		bool game_stop_observe_flag_ = false;
		bool game_set_commands_finish_flag = false;
		bool game_load_finish_flag_ = false;
		//virtual void OnGameStart() final;
		//virtual void OnStep() final;
		//todo control units by rules
		//virtual void OnUnitIdle(const Unit* u) final;
		//virtual void OnGameEnd() final;

		RunBot() {
			m_population.reserve(PopSize);

			//m_evaluators[0] = std::bind(&RunBot::simulate_single_solution_back_score, this, _1);
		}

		std::vector<std::vector<Solution>> turnPopToSolvec(Population population);

		void setGameInf(const std::map<sc2::Tag, sc2::Unit>& observed);

		void pushObservedUnits(const ObservationInterface*& ob);

		std::vector<std::pair<size_t, std::vector<MyScore>>> runMultiSolution(std::vector<std::vector<Solution>> load_solutions, State load_state = State());

		void setOrders(Command commands);

		sc2::Unit* findUnit(Tag tag);

		Point2D getCenterPos(const Units& units);

		Solution generateSolution(State load_state = State());

		Solution GA();

		void initializeAlgorithm();

		void initializePopulation();

		void calculateFitness();

		void select();

		void cross();

		void mutate();

	private:


		// generate a random solution
		//Solution generate_random_solution();


		//void evaluate_solutions(Population& p);


		// 算子部分
		float simulate_single_solution_back_score(const Solution& s);
	};

}

#endif