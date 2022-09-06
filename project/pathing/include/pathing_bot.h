#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "Mineral_TSP.h"
#include "GA_TSP.h"

#define UnitSize 22

using namespace sc2;

enum Game_Stage {
    //空环节
    Blank_Flag = 0,
    //节点更新环节
    Update_Flag = 1,
    //执行算法环节
    Algorithm_Flag = 2,
    //单元移动环节
    Move_Flag = 3,
    //游戏结束环节
    End_Flag = 4
};

//友元类前向声明
class Mineral_TSP;
class GA_TSP;

class PathingBot :public Agent {
private:
    int i = 0;
    Game_Stage m_game_stage = Update_Flag;
public:
    //友元类
    friend class Mineral_TSP;
    friend class GA_TSP;
    //Mineral_TSP方法的执行器，供Bot使用
    Mineral_TSP m_Actuator;
    //GA_TSP方法的执行器，供Bot使用
    GA_TSP m_GA;
    //通过算法得到的最优部署解
    std::vector<Node_TSP> m_best_solution;
    bool flag = false;
    bool is_end(const sc2::ObservationInterface* observation);
    int mineral_now = 0;

public:
    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }

    virtual void OnStep() final {
        //观测器
        const sc2::ObservationInterface* observation = Observation();
        //控制器
        sc2::ActionInterface* action = Actions();
        //单元容器
        sc2::Units units_vec = observation->GetUnits();

        mineral_now = observation->GetMinerals();

        if (mineral_max < observation->GetMinerals()) {
            mineral_max = observation->GetMinerals();
            std::cout << "GameLoop:" << observation->GetGameLoop() << '\t';
            std::cout << "Minerals:" << observation->GetMinerals() << std::endl;
            /*std::ofstream fout;
            fout.open("D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_bot.txt", std::ios::out | std::ios::app);
            fout << observation->GetGameLoop() << '\t' << observation->GetMinerals() << std::endl;*/
        }

        if (m_game_stage == Update_Flag) {
            if (m_Actuator.update_Unit(units_vec)) {
                m_game_stage = Algorithm_Flag;
                std::cout << "结点信息更新完毕，进行优化" << std::endl;
            }
        }

        if (m_game_stage == Algorithm_Flag) {
            m_GA.initialize_Algorithm(m_Actuator.get_mineral_vec());
            m_best_solution = m_GA.GA();
            //m_GA.GA();
            m_game_stage = Move_Flag;
        }
        std::vector<sc2::Point2D> point_vec;
        for (auto it : m_best_solution) {
            sc2::Point2D point(it.m_x, it.m_y);
            point_vec.push_back(point);
        }

        if (flag == true && mineral_now % ((UnitSize - 2) * 100) != 0) {
            flag = false;
        }

        if (!point_vec.empty()) {
            for (const auto&u : units_vec) {
                if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && (Distance2D(u->pos, point_vec.at(point_vec.size() - 1)) > 0.01f)) {
                    sc2::Point2D point_begin(u->pos.x, u->pos.y);
                    if (m_game_stage == Move_Flag && i != (point_vec.size() - 1)) {
                        action->UnitCommand(u, ABILITY_ID::MOVE, point_vec.at(i + 1));
                        i++;
                        m_game_stage = Blank_Flag;
                    }
                    else {
                        if (Distance2D(point_begin, point_vec.at(i)) < 1.5f) {
                            m_game_stage = Move_Flag;
                        }
                    }
                    if (flag == false && observation->GetMinerals() != 0 && observation->GetMinerals() % 2000 == 0) {
                        action->UnitCommand(u, ABILITY_ID::STOP);
                        point_vec.clear();
                        m_Actuator.get_mineral_vec().clear();
                        m_GA.~GA_TSP();
                        m_game_stage = Update_Flag;
                        flag = true;
                        mineral_now = observation->GetMinerals();
                        i = 0;
                        break;
                    }

                }
            }
        }


    }

    int mineral_max = 0;
};

bool PathingBot::is_end(const sc2::ObservationInterface* observation) {
    if (observation->GetMinerals() != 0 && observation->GetMinerals() % 2000 == 0) {
        return true;
    }
    else {
        return false;
    }
}


class NearestBot1Agent :public Agent {

private:
    sc2::Tag begin1_tag = 0;
    sc2::Tag begin2_tag = 0;
    bool move_flag1 = false;
    sc2::Point2D target1;
    int mineral_max = 0;
public:

    const sc2::Unit* get_nearest_unit(const sc2::Unit* begin_unit, const sc2::Units& units);
    const sc2::Unit* get_nearest_R2_unit(const sc2::Unit* begin_unit, const sc2::Units& units);
    int get_index_unit(const sc2::Unit* unit, const sc2::Units& units);


public:

    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }

    virtual void OnStep() final {
        //观测器
        const sc2::ObservationInterface* observation = Observation();
        //控制器
        sc2::ActionInterface* action = Actions();
        //单元容器
        sc2::Units units_vec = observation->GetUnits();

        if (mineral_max < observation->GetMinerals()) {
            mineral_max = observation->GetMinerals();
            std::cout << "GameLoop:" << observation->GetGameLoop() << '\t';
            std::cout << "Minerals:" << observation->GetMinerals() << std::endl;
            std::ofstream fout;
            fout.open("D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_nearest_bot.txt", std::ios::out | std::ios::app);
            fout << observation->GetGameLoop() << '\t' << observation->GetMinerals() << std::endl;
        }

        for (auto&u : units_vec) {
            if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE) {
                if (begin1_tag == 0) {
                    begin1_tag = u->tag;
                    continue;
                }
                if (begin2_tag == 0) {
                    begin2_tag = u->tag;
                    continue;
                }
            }
        }


        for (auto&u : units_vec) {
            const sc2::Unit* nearest_unit1_mark;
            if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && u->tag == begin1_tag) {
                if (!move_flag1) {
                    const sc2::Unit* nearest_unit1 = get_nearest_unit(u, units_vec);
                    nearest_unit1_mark = nearest_unit1;
                    if (nearest_unit1->tag != begin1_tag && nearest_unit1->tag != begin2_tag) {
                        action->UnitCommand(u, ABILITY_ID::MOVE, nearest_unit1->pos);
                        target1 = nearest_unit1->pos;
                        move_flag1 = true;
                    }

                    else {
                        const sc2::Unit* nearest_R2_unit1 = get_nearest_R2_unit(u, units_vec);
                        action->UnitCommand(u, ABILITY_ID::MOVE, nearest_R2_unit1->pos);
                        target1 = nearest_R2_unit1->pos;
                        move_flag1 = true;
                    }
                }

                if (sc2::Distance2D(sc2::Point2D(u->pos), target1) < 1.5f) {
                    move_flag1 = false;
                }
            }
        }
    }

};

const sc2::Unit* NearestBot1Agent::get_nearest_unit(const sc2::Unit* begin_unit, const sc2::Units& units) {

    int index_unit = get_index_unit(begin_unit, units);
    int distance_min = 100;
    int index_distance_min = 0;
    for (int i = 0; i < units.size(); i++) {
        float dist = sc2::Distance2D(units.at(index_unit)->pos, units.at(i)->pos);
        if (units.at(i)->unit_type == UNIT_TYPEID::TERRAN_MARINE)dist = 100;
        if (dist != 0 && dist < distance_min) {
            distance_min = dist;
            index_distance_min = i;
        }
    }

    return units.at(index_distance_min);

}

const sc2::Unit* NearestBot1Agent::get_nearest_R2_unit(const sc2::Unit* begin_unit, const sc2::Units& units) {

    int index_unit = get_index_unit(begin_unit, units);
    float distance_min = 100;
    int index_distance_min = 0;
    float distance_min_R2 = 100;
    int index_distance_min_R2 = 0;
    for (int i = 0; i < units.size(); i++) {
        float dist = sc2::Distance2D(units.at(index_unit)->pos, units.at(i)->pos);
        if (units.at(i)->unit_type == UNIT_TYPEID::TERRAN_MARINE)dist = 100;
        if (dist != 0 && dist < distance_min) {
            distance_min = dist;
            index_distance_min = i;
        }
    }

    for (int i = 0; i < units.size(); i++) {
        float dist = sc2::Distance2D(units.at(index_unit)->pos, units.at(i)->pos);
        if (units.at(i)->unit_type == UNIT_TYPEID::TERRAN_MARINE)dist = 100;
        if (dist != 0 && dist < distance_min_R2 && dist!= distance_min) {
            distance_min_R2 = dist;
            index_distance_min_R2 = i;
        }
    }

    std::cout << distance_min << " " << distance_min_R2 << std::endl;
    return units.at(index_distance_min_R2);

}

int NearestBot1Agent::get_index_unit(const sc2::Unit* unit, const sc2::Units& units) {

    int index = 0;

    for (auto u : units) {
        if (u == unit) {
            return index;
        }
        index++;
    }
    return -1;
}


class NearestBot2Agent :public Agent {

private:
    sc2::Tag begin1_tag = 0;
    sc2::Tag begin2_tag = 0;
    bool move_flag1 = false;
    bool move_flag2 = false;
    sc2::Point2D target1;
    sc2::Point2D target2;
    int mineral_max = 0;
public:

    const sc2::Unit* get_nearest_unit(const sc2::Unit* begin_unit, const sc2::Units& units);
    const sc2::Unit* get_nearest_R2_unit(const sc2::Unit* begin_unit, const sc2::Units& units);
    int get_index_unit(const sc2::Unit* unit, const sc2::Units& units);


public:

    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }

    virtual void OnStep() final {
        //观测器
        const sc2::ObservationInterface* observation = Observation();
        //控制器
        sc2::ActionInterface* action = Actions();
        //单元容器
        sc2::Units units_vec = observation->GetUnits();

        if (mineral_max < observation->GetMinerals()) {
            mineral_max = observation->GetMinerals();
            std::cout << "GameLoop:" << observation->GetGameLoop() << '\t';
            std::cout << "Minerals:" << observation->GetMinerals() << std::endl;
            std::ofstream fout;
            fout.open("D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_nearest_2agent.txt", std::ios::out | std::ios::app);
            fout << observation->GetGameLoop() << '\t' << observation->GetMinerals() << std::endl;
        }

        for (auto&u : units_vec) {
            if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE) {
                if (begin1_tag == 0) {
                    begin1_tag = u->tag;
                    continue;
                }
                if (begin2_tag == 0) {
                    begin2_tag = u->tag;
                    continue;
                }
            }
        }


        for (auto&u : units_vec) {
            const sc2::Unit* nearest_unit1_mark;
            if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && u->tag == begin1_tag) {
                if (!move_flag1) {
                    const sc2::Unit* nearest_unit1 = get_nearest_unit(u, units_vec);
                    nearest_unit1_mark = nearest_unit1;
                    if (nearest_unit1->tag != begin1_tag && nearest_unit1->tag != begin2_tag) {
                        action->UnitCommand(u, ABILITY_ID::MOVE, nearest_unit1->pos);
                        target1 = nearest_unit1->pos;
                        move_flag1 = true;
                    }

                    else {
                        const sc2::Unit* nearest_R2_unit1 = get_nearest_R2_unit(u, units_vec);
                        action->UnitCommand(u, ABILITY_ID::MOVE, nearest_R2_unit1->pos);
                        target1 = nearest_R2_unit1->pos;
                        move_flag1 = true;
                    }
                }

                if (sc2::Distance2D(sc2::Point2D(u->pos), target1) < 1.5f) {
                    move_flag1 = false;
                }
            }

            if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && u->tag == begin2_tag) {
                if (!move_flag2) {

                    const sc2::Unit* nearest_unit2 = get_nearest_unit(u, units_vec);
                    if (nearest_unit2->tag != begin1_tag && nearest_unit2->tag != begin2_tag && nearest_unit2->tag != nearest_unit1_mark->tag) {
                        action->UnitCommand(u, ABILITY_ID::MOVE, nearest_unit2->pos);
                        target2 = nearest_unit2->pos;
                        move_flag2 = true;
                    }

                    else {
                        const sc2::Unit* nearest_R2_unit2 = get_nearest_R2_unit(u, units_vec);
                        action->UnitCommand(u, ABILITY_ID::MOVE, nearest_R2_unit2->pos);
                        target2 = nearest_R2_unit2->pos;
                        move_flag2 = true;
                    }
                }

                if (sc2::Distance2D(sc2::Point2D(u->pos), target2) < 1.5f) {
                    move_flag2 = false;
                }
            }
        }
    }
};

const sc2::Unit* NearestBot2Agent::get_nearest_unit(const sc2::Unit* begin_unit, const sc2::Units& units) {

    int index_unit = get_index_unit(begin_unit, units);
    int distance_min = 100;
    int index_distance_min = 0;
    for (int i = 0; i < units.size(); i++) {
        float dist = sc2::Distance2D(units.at(index_unit)->pos, units.at(i)->pos);
        if (units.at(i)->unit_type == UNIT_TYPEID::TERRAN_MARINE)dist = 100;
        if (dist != 0 && dist < distance_min) {
            distance_min = dist;
            index_distance_min = i;
        }
    }

    return units.at(index_distance_min);

}

const sc2::Unit* NearestBot2Agent::get_nearest_R2_unit(const sc2::Unit* begin_unit, const sc2::Units& units) {

    int index_unit = get_index_unit(begin_unit, units);
    float distance_min = 100;
    int index_distance_min = 0;
    float distance_min_R2 = 100;
    int index_distance_min_R2 = 0;
    for (int i = 0; i < units.size(); i++) {
        float dist = sc2::Distance2D(units.at(index_unit)->pos, units.at(i)->pos);
        if (units.at(i)->unit_type == UNIT_TYPEID::TERRAN_MARINE)dist = 100;
        if (dist != 0 && dist < distance_min) {
            distance_min = dist;
            index_distance_min = i;
        }
    }

    for (int i = 0; i < units.size(); i++) {
        float dist = sc2::Distance2D(units.at(index_unit)->pos, units.at(i)->pos);
        if (units.at(i)->unit_type == UNIT_TYPEID::TERRAN_MARINE)dist = 100;
        if (dist != 0 && dist < distance_min_R2 && dist != distance_min) {
            distance_min_R2 = dist;
            index_distance_min_R2 = i;
        }
    }

    std::cout << distance_min << " " << distance_min_R2 << std::endl;
    return units.at(index_distance_min_R2);

}

int NearestBot2Agent::get_index_unit(const sc2::Unit* unit, const sc2::Units& units) {

    int index = 0;

    for (auto u : units) {
        if (u == unit) {
            return index;
        }
        index++;
    }
    return -1;
}



class RandBotAgent :public Agent {

private:
    sc2::Tag begin1_tag = 0;
    sc2::Tag begin2_tag = 0;
    bool move_flag1 = false;
    bool move_flag2 = false;
    sc2::Point2D target1;
    sc2::Point2D target2;
    int mineral_max = 0;
public:



public:

    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }

    virtual void OnStep() final {
        //观测器
        const sc2::ObservationInterface* observation = Observation();
        //控制器
        sc2::ActionInterface* action = Actions();
        //单元容器
        sc2::Units units_vec = observation->GetUnits();

        if (mineral_max < observation->GetMinerals()) {
            mineral_max = observation->GetMinerals();
            std::cout << "GameLoop:" << observation->GetGameLoop() << '\t';
            std::cout << "Minerals:" << observation->GetMinerals() << std::endl;
            std::ofstream fout;
            fout.open("D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_rand_2agent.txt", std::ios::out | std::ios::app);
            fout << observation->GetGameLoop() << '\t' << observation->GetMinerals() << std::endl;
        }

        for (auto&u : units_vec) {
            if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE) {
                if (begin1_tag == 0) {
                    begin1_tag = u->tag;
                    continue;
                }
                if (begin2_tag == 0) {
                    begin2_tag = u->tag;
                    continue;
                }
            }
        }

        for (auto&u : units_vec) {
            if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && u->tag == begin1_tag) {
                if (!move_flag1) {
                    target1 = units_vec.at(rand() % units_vec.size())->pos;
                    action->UnitCommand(u, ABILITY_ID::MOVE, target1);
                    move_flag1 = true;
                }

                if (sc2::Distance2D(sc2::Point2D(u->pos), target1) < 1.5f) {
                    move_flag1 = false;
                }
            }

            if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && u->tag == begin2_tag) {
                if (!move_flag2) {
                    target2 = units_vec.at(rand() % units_vec.size())->pos;
                    action->UnitCommand(u, ABILITY_ID::MOVE, target2);
                    move_flag2 = true;
                }

                if (sc2::Distance2D(sc2::Point2D(u->pos), target2) < 1.5f) {
                    move_flag2 = false;
                }
            }
        }
    }

};
