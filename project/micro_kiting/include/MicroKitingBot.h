#pragma once

#include "sc2api/sc2_api.h"
#include <iostream>
#include <fstream>

namespace sc2 {
struct TrainData {
    Point2D self_pos;
    Point2D enemy_pos;
    Point2D target_pos;

    TrainData(Point2D sp, Point2D ep, Point2D tp) :self_pos(sp), enemy_pos(ep), target_pos(tp) {}
    bool operator==(const TrainData& rhs) const {
        return target_pos == rhs.target_pos;
    }
};

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
    void printGoodTrainedData() {
        std::ofstream fout0;
        fout0.open("D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/trained/good_trained_data.txt", std::ios::app);
        for (int i = 0; i < train_data_vec.size(); ++i) {
            fout0 << train_data_vec[i].self_pos.x << '\t' << train_data_vec[i].self_pos.y << '\t';
            fout0 << train_data_vec[i].enemy_pos.x << '\t' << train_data_vec[i].enemy_pos.y << '\t';
            fout0 << train_data_vec[i].target_pos.x << '\t' << train_data_vec[i].target_pos.y << std::endl;
        }
        fout0.close();
    }
    void printBadTrainedData() {
        std::ofstream fout0;
        fout0.open("D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/trained/bad_trained_data.txt", std::ios::app);
        for (int i = 0; i < train_data_vec.size(); ++i) {
            fout0 << train_data_vec[i].self_pos.x << '\t' << train_data_vec[i].self_pos.y << '\t';
            fout0 << train_data_vec[i].enemy_pos.x << '\t' << train_data_vec[i].enemy_pos.y << '\t';
            fout0 << train_data_vec[i].target_pos.x << '\t' << train_data_vec[i].target_pos.y << std::endl;
        }
        fout0.close();
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
    std::vector<TrainData> train_data_vec;
};


}


