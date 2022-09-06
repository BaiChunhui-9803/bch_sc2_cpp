#pragma once
#include "sc2api/sc2_api.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>

#include "GA_micro.h"

using namespace sc2;

enum Game_Stage {
    //�ջ���
    Blank_Flag = 0,
    //��Ϸ��Ϣ���»���
    Update_Flag = 1,
    //ִ���㷨����
    Algorithm_Flag = 2,
    //��Ϸ���ƻ���
    Action_Flag = 3,
    //��Ϸ��������
    End_Flag = 10
};

//��Ԫ��ǰ������
class GA_TSP;

class MicroAgentBot : public sc2::Agent {

private:
    Game_Stage m_game_stage = Update_Flag;
    GameInfomation m_game_info;
    std::vector<MAP_MYSOL> m_best_sol;
    //std::_Tree_iterator<class std::_Tree_val<struct std::_Tree_simple_types<struct std::pair<unsigned int const, struct std::pair<enum sc2::ABILITY_ID, struct sc2::Point3D>>>>> m_it_loop;
    std::vector<Gameloop> m_loop_vec;
    int m_loop_tag;

    const Unit* targeted_zergling_;
    //�ƶ���־λ
    bool move_back_;
    Point2D backup_target_;
    Point2D backup_start_;
    bool game_win_ = false;
    bool Update_Enemy_Flag = false;
    double m_self_health_max;
    double m_enemy_health_max;
    double m_self_health_result;
    double m_enemy_health_result;
    double m_fitness = 0.0;

public:
    //��Ԫ��
    friend class GA_micro;
    GA_micro m_Al;
    bool update_GameInfo(sc2::Units units);

    inline double get_Self_Max() { return m_self_health_max; }
    inline double get_Enemy_Max() { return m_enemy_health_max; }
    inline void set_Self_Max(const double health_max) { m_self_health_max = health_max; }
    inline void set_Enemy_Max(const double health_max) { m_enemy_health_max = health_max; }
    inline double get_Self_Result() { return m_self_health_result; }
    inline double get_Enemy_Result() { return m_enemy_health_result; }
    inline void set_Self_Result(const double health_result) { m_self_health_result = health_result; }
    inline void set_Enemy_Result(const double health_result) { m_enemy_health_result = health_result; }
    inline double get_Fitness() { return m_fitness; }
    inline void set_Fitness(const double fitness) { m_fitness = fitness; }

    inline bool get_Game_Ended_() { return game_win_; }
    void set_Game_Ended_(sc2::GameResult game_result);


public:
    // ��Ϸ����ִ��
    virtual void OnGameStart() final;
    // ��Ϸ��ִ֡�� Your bots OnStep function will be called each time the coordinator steps the simulation forward.
    virtual void OnStep() final;
    // ��λ���ݻ�ִ��
    virtual void OnUnitDestroyed(const Unit* unit) override;

private:
    // ��ȡ��λΪalliace��ƽ��position ����Point2D& position
    bool GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position);
    // ��ȡ������嵥λ ����Point2D& from
    bool GetNearestZergling(const Point2D& from);
};