#pragma once

#include "sc2api/sc2_api.h"
#include <iostream>

namespace sc2 {

class MicroKitingBot : public sc2::Agent {
public:
    // 游戏启动执行
    virtual void OnGameStart() final;
    // 游戏走帧执行 Your bots OnStep function will be called each time the coordinator steps the simulation forward.
    virtual void OnStep() final;
    // 单位被摧毁执行
    virtual void OnUnitDestroyed(const Unit* unit) override;

    inline bool get_Game_Ended_() {
        return game_win_;
    }
    inline void set_Game_Ended_(sc2::GameResult game_result) {
        if (game_result == sc2::GameResult::Win) game_win_ = true;
        else game_win_ = false;
    }
private:
    // 获取单位为alliace的平均position 更新Point2D& position
    bool GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position);
    // 获取最近虫族单位 更新Point2D& from
    bool GetNearestZergling(const Point2D& from);

    //攻击目标单位的指针Unit*
    const Unit* targeted_zergling_;
    //移动标志位
    bool move_back_;
    Point2D backup_target_;
    Point2D backup_start_;
    bool game_win_ = false;
    bool Update_Enemy_Flag = false;
};


}


