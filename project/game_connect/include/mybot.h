#ifndef MYBOT_H
#define MYBOT_H

#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2renderer/sc2_renderer.h"
#include <iostream>
#include <map>

#include "State.h"

namespace sc2 {

struct MyScore {
    float m_damage_to_enemy = 0.0;
    float m_damage_to_self = 0.0;
    float m_total_score = 0.0;

    MyScore() = default;
    MyScore(float d2e, float d2s, float ts) :m_damage_to_enemy(d2e), m_damage_to_self(d2s), m_total_score(ts) {}
};

struct MyConnectBot : public sc2::Agent {
public:
    // 游戏启动执行
    virtual void OnGameStart() final;
    // 游戏走帧执行 Your bots OnStep function will be called each time the coordinator steps the simulation forward.
    virtual void OnStep() final;

    size_t m_count = 0;
    size_t m_num = 0;
    bool has_save = false;
    State save_state = State();
    bool game_stop_observe_flag_ = false;
    bool game_pause_flag_ = false;
    bool game_save_flag_ = false;
    bool game_load_flag_ = false;
    bool game_load_finish_flag_ = false;
    bool game_finish_flag_ = false;
    bool game_idle_flag = false;
    bool game_leave_flag = false;
    bool game_set_commands_finish_flag = false;
    bool flag_test = false;
    bool game_rerun_flag_ = false;

    std::map<sc2::Tag, sc2::Unit> observed_units;
    Units observed_self_units;
    Units observed_enemy_units;
    std::vector<MyScore> m_scorer_vec;
private:
    size_t begin_selfN = 0;
    size_t begin_enemyN = 0;
    double begin_selfHP = 0.0;
    double begin_enemyHP = 0.0;
    ActionInterface* action = Actions();

public:

    void pushObservedUnits(const ObservationInterface*& ob);

    void setGameInf(const std::map<sc2::Tag, sc2::Unit>& observed);

    void setGameInf(const State& state);

    void setGameInf(double bsHP, double beHP, size_t bsN, size_t beN);

    void showGameInf();

    // not used now
    void getGameInf();

    std::vector<MyScore> getScore();

    Point2D getCenterPos(const Units& units);

    void setOrders(Command commands);

    sc2::Unit* findUnit(Tag tag);

    //sc2::Unit
};


}


#endif