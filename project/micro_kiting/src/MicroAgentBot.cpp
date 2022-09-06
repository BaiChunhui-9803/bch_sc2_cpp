#include "sc2api/sc2_api.h"
#include "MicroAgentBot.h"
#include <iostream>

#define EnemySize 6

using namespace sc2;

void MicroAgentBot::OnGameStart() {
	std::cout << "Hello, World!";
	move_back_ = false;
	targeted_zergling_ = 0;
}

void MicroAgentBot::OnStep() {
	//观测器
	const sc2::ObservationInterface* observation = Observation();
	//控制器
	sc2::ActionInterface* action = Actions();
	//单元容器
	sc2::Units units_vec = observation->GetUnits();
	sc2::Units units_vec_enemy = observation->GetUnits(Unit::Alliance::Enemy);
	sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
	//输出窗口
	sc2::DebugInterface* debug = Debug();
	sc2::Point2D debug_point(0, 0.2);
	sc2::Point2D debug_point_delta_x(0.01, 0);
	sc2::Point2D debug_point_delta_y(0, 0.01);
	debug->DebugTextOut("GameLoop:" + std::to_string(observation->GetGameLoop()));
	debug->DebugTextOut("MAP:MarineMicro", debug_point, Colors::White);
	debug->DebugTextOut("PROBLEM:Micro", debug_point + debug_point_delta_y * 2, Colors::White);
	debug->DebugTextOut("ALGORITHM:GA", debug_point + debug_point_delta_y * 4, Colors::White);

	//std::cout << "观测敌方数量：" << units_vec_enemy.size() << std::endl;
	//敌方加载需要时间，需要对观测器中敌方信息给予判断
	if (m_game_stage == Update_Flag && units_vec_enemy.size() == EnemySize) {
		if (update_GameInfo(units_vec)) {
			m_game_stage = Algorithm_Flag;
			std::cout << "单元信息更新完毕，进行优化" << std::endl;
			action->SendChat("Updating done, now perform optimization.");

			double sum_self_max = 0.0;
			double sum_enemy_max = 0.0;
			for (auto& ue : units_vec_enemy) {
				sum_enemy_max += ue->health_max;
			}
			for (auto& us : units) {
				sum_self_max += us->health_max;
			}
			set_Self_Max(sum_self_max);
			set_Enemy_Max(sum_enemy_max);
		}
	}

	if (m_game_stage == Algorithm_Flag) {
		m_Al.initialize_Algorithm(m_game_info);
		m_best_sol = m_Al.get_random_solution_vec();
		m_Al.Algoritrm();
		m_game_stage = Action_Flag;
		for (auto it = m_best_sol.at(0).begin(); it != m_best_sol.at(0).end(); it++) {
			std::cout << it->first << " " << int(it->second.first) << " " << it->second.second.x << " " << it->second.second.y << " " << it->second.second.z << std::endl;
		}
		for (auto i : m_best_sol.at(0)) {
			m_loop_vec.push_back(i.first);
		}
		m_loop_tag = 0;
	}

	if (!units.empty()) {
		double sum_self_health = 0.0;
		for (auto& us : units) {
			sum_self_health += us->health;
		}
		set_Self_Result(sum_self_health);
	}

	if (!units_vec_enemy.empty()) {
		double sum_enemy_health = 0.0;
		for (auto& ue : units_vec_enemy) {
			sum_enemy_health += ue->health;
		}
		set_Enemy_Result(sum_enemy_health);
	}

	/*if (observation->GetGameLoop == m_best_sol.at(0)) {
		system("pause");
		auto a = m_best_sol.at(0).at;
	}*/

	//auto it_loop = m_best_sol.at(0).begin();
	//std::cout << typeid(it_loop).name() << std::endl;

	Point2D mp, zp;
	if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
		return;
	}
	if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
		return;
	}
	if (!GetNearestZergling(mp)) {
		return;
	}

	//for (const auto& u : units) {
	//	switch (static_cast<UNIT_TYPEID>(u->unit_type)) {
	//	case UNIT_TYPEID::TERRAN_MARINE: {
	//		/*if (observation->GetGameLoop() == m_it_loop->first) {
	//			action->UnitCommand(u, ABILITY_ID::SMART, m_it_loop->second.second);
	//		}*/
	//		if (!move_back_) {
	//			if (observation->GetGameLoop() >= m_loop_vec.at(m_loop_tag)) {
	//				action->UnitCommand(u, ABILITY_ID::SMART, m_best_sol.at(0).find(m_loop_vec.at(m_loop_tag))->second.second);
	//			}
	//			//m_game_stage = Blank_Flag;
	//		}
	//		else {
	//			if (m_loop_vec.size() != 0 && m_loop_tag != (m_loop_vec.size() - 1)) {
	//				if (observation->GetGameLoop() >= m_loop_vec.at(m_loop_tag + 1)) {
	//					m_loop_tag++;
	//					m_game_stage = Action_Flag;
	//				}
	//			}
	//		}
	//		break;
	//	}
	//	default: {
	//		break;
	//	}
	//	}
	//}
	//m_it_loop++;

	if (m_game_stage == Action_Flag) {
		for (const auto& u : units) {
			switch (static_cast<UNIT_TYPEID>(u->unit_type)) {
			case UNIT_TYPEID::TERRAN_MARINE: {
				if (!move_back_) {
					action->UnitCommand(u, ABILITY_ID::ATTACK, targeted_zergling_);

				}
				else {
					if (Distance2D(mp, backup_target_) < 1.5f) {
						move_back_ = false;
						m_loop_tag++;
					}

					action->UnitCommand(u, ABILITY_ID::SMART, backup_target_);
				}
				break;
			}
			default: {
				break;
			}
			}
		}
	}

	debug->SendDebug();

	//敌人数量降至0时，因某种原因不再进行OnStep循环，故在1时进行判断
	if (units_vec_enemy.size() == 1 && Update_Enemy_Flag) {
		set_Game_Ended_(sc2::GameResult::Win);
		std::ofstream fout;
		fout << "game end" << std::endl;
	}
}

void MicroAgentBot::OnUnitDestroyed(const Unit* unit) {
	if (unit == targeted_zergling_) {
		Point2D mp, zp;
		if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
			return;
		}

		if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
			set_Game_Ended_(sc2::GameResult::Win);
			std::ofstream fout;
			fout << "game end" << std::endl;
			return;
		}

		Vector2D diff = mp - zp;
		Normalize2D(diff);

		targeted_zergling_ = 0;
		move_back_ = true;
		backup_start_ = mp;
		//backup_target_ = mp + diff * 3.0f;
		if (m_loop_tag != (m_loop_vec.size() - 1)) {
			backup_target_ = m_best_sol.at(0).at(m_loop_vec.at(m_loop_tag)).second;
		}
		else {
			backup_target_ = mp;
		}
	}
}


bool MicroAgentBot::update_GameInfo(sc2::Units units) {

	for (auto u : units) {
		NodeUnit node_unit;
		node_unit.n_alliance = u->alliance;
		node_unit.n_tag = u->tag;
		node_unit.n_type = u->unit_type;
		node_unit.n_pos = u->pos;
		m_game_info.vec_unit.push_back(node_unit);
	}

	std::ofstream fout;
	fout.open("D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/unit_info_gb.txt");
	for (auto it : m_game_info.vec_unit) {
		fout << it.n_alliance << '\t' << it.n_tag << '\t' << it.n_type << '\t' << it.n_pos.x << '\t' << it.n_pos.y << '\t' << it.n_pos.z << std::endl;
	}

	fout.close();

	return true;
}

bool MicroAgentBot::GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position) {
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits(alliace);

	if (units.empty()) {
		return false;
	}

	position = Point2D(0.0f, 0.0f);
	unsigned int count = 0;

	for (const auto& u : units) {
		if (u->unit_type == unit_type) {
			position += u->pos;
			++count;
		}
	}

	position /= (float)count;

	return true;
}

bool MicroAgentBot::GetNearestZergling(const Point2D& from) {
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits(Unit::Alliance::Enemy);

	if (units.empty()) {
		return false;
	}

	float distance = std::numeric_limits<float>::max();
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::ZERG_ZERGLING) {
			float d = DistanceSquared2D(u->pos, from);
			if (d < distance) {
				distance = d;
				targeted_zergling_ = u;
			}
		}
	}

	return true;
}

void MicroAgentBot::set_Game_Ended_(sc2::GameResult game_result) {
	if (game_result == sc2::GameResult::Win) {
		game_win_ = true;
		double fitness = (get_Enemy_Max() - get_Enemy_Result()) - (get_Self_Max() - get_Self_Result());
		set_Fitness(fitness);
		std::cout << "此次模拟的适应值：" << get_Fitness() << std::endl;
		std::ofstream fout;
		fout.open("D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/kiting_fitness_noneAL.txt",std::ios::app);
		fout << get_Fitness() << std::endl;
		fout.close();
	}
	else game_win_ = false;
}
