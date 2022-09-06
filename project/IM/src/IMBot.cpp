#include <iostream>
#include <fstream>

#include "sc2api/sc2_api.h"

#include "IMBot.h"


namespace sc2 {

	void IMBot::OnGameStart() {
		move_back_ = false;
		targeted_enemy_ = 0;
		//观测器
		const ObservationInterface* static_observation = Observation();
		//控制器
		ActionInterface* static_action = Actions();
		//单元容器
		UnitsVec static_units_vec = static_observation->GetUnits();
		UnitsVec static_units_vec_self = static_observation->GetUnits(sc2::Unit::Alliance::Self);
		UnitsVec static_units_vec_enemy = static_observation->GetUnits(Unit::Alliance::Enemy);
		//****************设置游戏基本参数****************//
		m_game_set.updateGameSet(static_units_vec_self, static_units_vec_enemy);
		//***********************************************//
	}

	void IMBot::OnStep() {
		//观测器
		const ObservationInterface* observation = Observation();
		//控制器
		ActionInterface* action = Actions();
		//单元容器
		m_units_vec = observation->GetUnits();
		m_units_vec_self = observation->GetUnits(sc2::Unit::Alliance::Self);
		m_units_vec_enemy = observation->GetUnits(Unit::Alliance::Enemy);
		sc2::DebugInterface* debug = Debug();
		/***********************************************/
		/****************传递/更新/存储IM****************/
		InfluenceMap IM2(m_units_vec_self, Self);
		InfluenceMap IM3(m_units_vec_enemy, Enemy);
		m_IMptr_list.push_back(std::make_unique<IMNode>(boost::make_tuple(
			0.0f,
			observation->GetGameLoop(),
			this->getHPSelf(m_units_vec_self) / m_game_set.getMaxHPSelf(),
			//this->getHPSelf(m_units_vec_self),
			this->getHPEnemy(m_units_vec_enemy) / m_game_set.getMaxHPEnemy(),
			//(m_game_set.getMaxHPEnemy() - this->getHPEnemy(m_units_vec_enemy)),
			InfluenceMap(m_units_vec, Neutral))));
		m_IMptr_list.back()->get<4>().updateIMValue(m_units_vec);

#ifdef _DRAW_
		//IM2.updateIMValue(m_units_vec_self);
		//IM3.updateIMValue(m_units_vec_enemy);
		m_IMptr_list.back()->get<4>().writeIMarrToFile(fOutPathIMarr, fOutPathIMarr3r);
		//IM2.writeIMarrToFile(fOutPathIMarrSelf, fOutPathIMarrSelf3r);
		//IM3.writeIMarrToFile(fOutPathIMarrEnemy, fOutPathIMarrEnemy3r);
		//std::cout << IM;
		if (display_cnt % 100 == 0) {
		displayIMarr();
		}
#endif
		/***********************************************/
		/*******更新敌人群落/算法初始化/传递游戏信息*******/
		m_IM_pop = updateIMPop(m_units_vec_enemy);
		if (m_game_stage == Update_Flag) {
			m_game_stage = Action_Flag;
		}
		updateGameInfo(m_units_vec);

		MapPoint center_self = getCenterSelf(m_units_vec_self);
		for (auto &u : m_game_info.info_vec_unit) {
			if (u.n_alliance == Enemy) {
				float distance = m_IM.calculateWeightDistance(
					m_IM_pop,
					u.n_pop,
					center_self,
					MapPoint(u.n_pos));
				debug->DebugTextOut(std::to_string(u.n_pop) + ":" + std::to_string(distance), u.n_pos);
			}
		}
		//debug->DebugMoveCamera(center_self);
		debug->DebugMoveCamera(backup_target_);
		debug->SendDebug();

		if (gameloop_cnt % 10 == 0) updatePerGameLoop(gameloop_cnt);
		//for (int i = 0; i < m_IM_pop.size(); ++i) {
		//	std::cout << "IM_pop-" << i << "(" << m_IM_pop.at(i).first.x << "," << m_IM_pop.at(i).first.y << ")\n";
		//}
		//for (int i = 0; i < m_game_info.info_vec_unit.size(); ++i) {

		//	std::cout << i << "-gridpos:" << m_IM.turnMapToGrid(MapPoint(m_game_info.info_vec_unit.at(i).n_pos)) << "\t";
		//	std::cout << i << "-pop:" << m_game_info.info_vec_unit.at(i).n_pop << "\n";
		//}

		m_AL.initializeAlgorithm(m_game_info, m_IM_pop);
		if (m_lock == false) {
			targeted_enemy_tag_ = m_AL.findNearsetPoint();
			m_lock = true;
		}

		for (int i = 0; i < m_units_vec_enemy.size(); ++i) {
			if (m_units_vec_enemy.at(i)->tag == targeted_enemy_tag_) {
				targeted_enemy_ = m_units_vec_enemy.at(i);
			}
		}

		MapPoint point_begin=m_AL.getCenterSelf();


		//targeted_enemy_tag_ = m_AL.findNearsetPoint();
		if (m_game_stage == Action_Flag) {
			for (const auto& u : m_units_vec_self) {
				switch (static_cast<UNIT_TYPEID>(u->unit_type)) {
				case UNIT_TYPEID::TERRAN_MARINE: {
					if (!move_back_) {
						action->UnitCommand(u, ABILITY_ID::ATTACK, targeted_enemy_);

					}
					else {
						if (Distance2D(point_begin, backup_target_) < 1.5f) {
							move_back_ = false;
							//m_loop_tag++;
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

		//敌人数量降至0时，因某种原因不再进行OnStep循环，故在1时进行判断
		if (m_units_vec_enemy.size() == 0) {
			set_Game_Ended_(sc2::GameResult::Win);
		}

		writeIMptrList();
		m_game_info.info_vec_unit.clear();

		/***********************************************/
		/********************变量自增********************/
		//游戏循环计数器
		++display_cnt;
 		++gameloop_cnt;
		/***********************************************/
	}
}

void sc2::IMBot::OnUnitDestroyed(const Unit* unit) {
	//更新游戏信息
	m_units_vec = Observation()->GetUnits();
	m_units_vec_self = Observation()->GetUnits(sc2::Unit::Alliance::Self);
	m_units_vec_enemy = Observation()->GetUnits(Unit::Alliance::Enemy);

	//敌人数量降至0时，因某种原因不再进行OnStep循环，故在1时进行判断
	if (m_units_vec_enemy.size() == 0) {
		set_Game_Ended_(sc2::GameResult::Win);
	}
	//if (unit->tag == targeted_enemy_tag_) {
	if (unit->alliance == Enemy && !get_Game_Ended_()) {
		m_IM_pop = updateIMPop(m_units_vec_enemy);
		updateGameInfo(m_units_vec);
		m_AL.updateAlgorithm(m_game_info, m_IM_pop);
		targeted_enemy_tag_ = m_AL.findNearsetPoint();

		//计算新位置
		MapPoint point_begin = m_AL.getCenterSelf();
		MapPoint point_enemy = m_AL.getKiteMapPoint(m_AL.getPopIndex(targeted_enemy_tag_));
		for (int i = 0; i < m_units_vec_enemy.size(); ++i) {
			if (m_units_vec_enemy.at(i)->tag == targeted_enemy_tag_) {
				targeted_enemy_ = m_units_vec_enemy.at(i);
			}
		}
		//if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
		//	set_Game_Ended_(sc2::GameResult::Win);
		//	return;
		//}

		Vector2D diff = point_begin - point_enemy;
		Normalize2D(diff);

		targeted_enemy_ = 0;
		move_back_ = true;
		backup_start_ = point_begin;
		backup_target_ = point_enemy;
		//backup_target_ = mp + diff * 3.0f;
		//if (m_loop_tag != (m_loop_vec.size() - 1)) {
		//	backup_target_ = m_best_sol.at(0).at(m_loop_vec.at(m_loop_tag)).second;
		//}
		//else {
		//	backup_target_ = mp;
		//}
	}

	//m_game_stage = Kite_Flag;
	//m_target_kite = m_AL.getKiteMapPoint(m_AL.getPopIndex(m_target_tag));
	//m_lock = false;
	////if (unit->tag == m_target_tag) {
	////	m_lock = false;
	////	m_game_stage = Move_Flag;
	////}


}

std::ostream& operator<<(std::ostream& os, const InfluenceMap& IM) {
	os << "IM:" << std::endl;
	os << "IM:m_map_position:" << IM.getMapPosition().getCenter() << std::endl;
	os << "IM:m_map_alliance:" << IM.getMapAlliance() << std::endl;
	os << "IM:m_map_arr:" << std::endl;
	os << IM.getMapArray();
	return os;
}

std::ostream& operator<<(std::ostream& os, const arrint2D* arr) {
	for (int i = 0; i < MAP_GRID_SIZE; ++i) {
		for (int j = 0; j < MAP_GRID_SIZE; ++j) {
			os << arr[i][j] << " ";
		}
		os << std::endl;
	}
	return os;
}


std::ostream& operator<<(std::ostream& os, const GridPoint& grid_point) {
	os << "(" << grid_point.x << "," << grid_point.y << ")";
	return os;
}

std::ostream & operator<<(std::ostream & os, const MapPoint & map_point)
{
	os << "(" << map_point.x << "," << map_point.y << ")";
	return os;
}

bool sc2::IMBot::updateGameInfo(const UnitsVec& units) {
	for (auto &u : units) {
		NodeUnit node_unit;
		node_unit.n_alliance = u->alliance;
		node_unit.n_tag = u->tag;
		node_unit.n_type = u->unit_type;
		node_unit.n_pos = u->pos;
		if (node_unit.n_alliance == Enemy) node_unit.n_pop = getIMPop(MapPoint(u->pos));
		else node_unit.n_pop = -1;
		m_game_info.info_vec_unit.push_back(node_unit);
	}
	return true;
}

const HPself sc2::IMBot::getHPSelf(const UnitsVec& units) const {
	HPself value = 0.0f;
	for (auto& u : units) {
		value += u->health;
	}
	return value;
}

const HPenemy sc2::IMBot::getHPEnemy(const UnitsVec& units) const {
	HPenemy value = 0.0f;
	for (auto& u : units) {
		value += u->health;
	}
	return value;
}

bool sc2::IMBot::updatePerGameLoop(const Gameloop loop_cnt) {
	for (auto it = this->m_IMptr_list.begin(); it != this->m_IMptr_list.end(); ++it) {
		(*it)->head = float((*it)->get<1>()) / float(loop_cnt);
	}
	return true;
}

//此处有bug，popvec无限大
IMPopVec sc2::IMBot::updateIMPop(const UnitsVec& units_vec_enemy) {
	int gridmap[MAP_GRID_SIZE][MAP_GRID_SIZE] = {};
	IMPopVec popvec = {};
	int i = 0;
	for (auto &u : units_vec_enemy) {
		GridPoint gridpoint = m_IM.turnMapToGrid(MapPoint(u->pos));
		std::vector<int> beighbor_vec = findBeighborsIn2(gridpoint, gridmap);
		if (isNewPop(gridpoint, beighbor_vec)) {
			++i;
			gridmap[gridpoint.x][gridpoint.y] = i;
			popvec.push_back(std::pair<GridPoint, size_t>(GridPoint(gridpoint.x, gridpoint.y), 0));
		}
		//else {
		//	++popvec.at(i).second;
		//}

	}
	return popvec;
}

bool sc2::IMBot::isNewPop(const GridPoint& grid_point, const std::vector<int>& vec) {
	for (int i = 0; i < vec.size(); ++i) {
		if (vec.at(i) != 0) return false;
	}
	return true;
}

std::vector<int> sc2::IMBot::findBeighborsIn2(const GridPoint& grid_point, arrint2D* arr_ptr) {
	int max_x = MAP_GRID_SIZE - 1;
	int max_y = MAP_GRID_SIZE - 1;
	std::vector<int> list;
	list.push_back(arr_ptr[grid_point.x][grid_point.y]);
	for (int dx = (grid_point.x > 0 ? -2 : 0); dx <= (grid_point.x < max_x ? 2 : 0); ++dx) {
		for (int dy = (grid_point.y > 0 ? -2 : 0); dy <= (grid_point.y < max_y ? 2 : 0); ++dy) {
			if (dx != 0 || dy != 0) {
				list.push_back(arr_ptr[grid_point.x + dx][grid_point.y + dy]);
			}
		}
	}

	return list;
}

IMPopId sc2::IMBot::getIMPop(const MapPoint& map_point) {
	GridPoint gridppint = m_IM.turnMapToGrid(map_point);
	int id;
	float distance_min = 99.0f;
	for (int i = 0; i < m_IM_pop.size(); ++i) {
		float distance = sc2::Distance2D(MapPoint(gridppint.x, gridppint.y), MapPoint(m_IM_pop.at(i).first.x, m_IM_pop.at(i).first.y));
		if (distance < distance_min) {
			distance_min = distance;
			id = i;
		}
	}
	++m_IM_pop.at(id).second;
	return id;
}

void IMBot::set_Game_Ended_(sc2::GameResult game_result) {
	if (game_result == sc2::GameResult::Win) {
		game_win_ = true;
		//double fitness = (get_Enemy_Max() - get_Enemy_Result()) - (get_Self_Max() - get_Self_Result());
		//set_Fitness(fitness);
		//std::cout << "此次模拟的适应值：" << get_Fitness() << std::endl;
		updatePerGameLoop(gameloop_cnt);
	}
	else game_win_ = false;
}

void sc2::IMBot::writeIMptrList() {
	std::ofstream fout1;
	fout1.open(fOutPathIMptrList);
	for (auto it = m_IMptr_list.begin(); it != m_IMptr_list.end(); ++it) {
		fout1 << (*it)->get<0>() << "\t" << (*it)->get<1>() << "\t" << (*it)->get<2>() << "\t" << (*it)->get<3>() << std::endl;
	}
}

MapPoint sc2::IMBot::getCenterSelf(const UnitsVec & units) {
	MapPoint point_sum = MapPoint();
	for (auto &u : units) {
		point_sum += MapPoint(u->pos);
	}
	return point_sum / units.size();
}