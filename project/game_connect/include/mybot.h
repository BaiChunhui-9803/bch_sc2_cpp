#pragma once

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

    MyScore(float d2e, float d2s, float ts) :m_damage_to_enemy(d2e), m_damage_to_self(d2s), m_total_score(ts) {}
};

struct MyConnectBot : public sc2::Agent {
public:
    // 游戏启动执行
    virtual void OnGameStart() final;
    // 游戏走帧执行 Your bots OnStep function will be called each time the coordinator steps the simulation forward.
    virtual void OnStep() final;

    State save_state;
    bool game_stop_observe_flag_ = false;
    bool game_pause_flag_ = false;
    bool game_save_flag_ = false;
    bool game_load_flag_ = false;
    bool game_load_finish_flag_ = false;

    std::map<sc2::Tag, sc2::Unit> observed_units;
private:
    size_t begin_selfN = 0;
    size_t begin_enemyN = 0;
    double begin_selfHP = 0.0;
    double begin_enemyHP = 0.0;
public:

    void pushObservedUnits(const ObservationInterface*& ob);

    void setGameInf(const std::map<sc2::Tag, sc2::Unit>& observed);

    void setGameInf(const State& state);

    void setGameInf(double bsHP, double beHP, size_t bsN, size_t beN);

    void showGameInf();

    // not used now
    void getGameInf();

    MyScore getScore();

    Point2D getCenterPos(const Units& units);
};


}


