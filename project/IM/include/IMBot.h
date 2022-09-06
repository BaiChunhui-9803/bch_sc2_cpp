#ifndef _IMBOT_H_
#define _IMBOT_H_
#include <iostream>
#include <memory>
#include <map>

#include "sc2api/sc2_api.h"
#include "InfluenceMap.h"
#include "MyAlgorithm.h"

typedef float PerGameloop;
typedef float HPself;
typedef float HPenemy;
typedef float HPselfdM;
typedef float HPenemydM;
typedef std::vector<InfluenceMap> IMVec;
typedef std::vector<std::pair<GridPoint, size_t>> IMPopVec;
typedef boost::tuple<PerGameloop, Gameloop, HPselfdM, HPenemydM, InfluenceMap> IMNode;
typedef std::unique_ptr<boost::tuple<PerGameloop, Gameloop, HPselfdM, HPenemydM, InfluenceMap>> IMNode_ptr;
typedef std::list<std::unique_ptr<boost::tuple<PerGameloop, Gameloop, HPselfdM, HPenemydM, InfluenceMap>>> IMptrList;

//��д�ļ�·��
static const std::string fOutPathIMptrList = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IMptr_list.txt";

std::ostream& operator<<(std::ostream& os, const InfluenceMap& IM);
std::ostream& operator<<(std::ostream& os, const arrint2D* arr);
std::ostream& operator<<(std::ostream& os, const GridPoint& grid_point);
std::ostream& operator<<(std::ostream& os, const MapPoint& map_point);

enum Game_Stage {
    //�ջ���
    Blank_Flag = 0,
    //�ڵ���»���
    Update_Flag = 1,
    //ִ���㷨����
    Algorithm_Flag = 2,
    //��Ԫ�ƶ�����
    Action_Flag = 3,
    //��Ϸ��������
    End_Flag = 7
};

struct GameSet {
private:
    HPself max_HPself = 0.0f;
    HPenemy max_HPenemy = 0.0f;

public:
    GameSet() :max_HPself(0.0f), max_HPenemy(0.0f) {};
    const HPself getMaxHPSelf() const { return max_HPself; }
    const HPenemy getMaxHPEnemy() const { return max_HPenemy; }
    void updateGameSet(const UnitsVec& units_vec_self, const UnitsVec& units_vec_enemy) {
        max_HPself = 0;
        max_HPenemy = 0;
        for (auto &u : units_vec_self) max_HPself += u->health_max;
        for (auto &u : units_vec_enemy) max_HPenemy += u->health_max;
    }
};

namespace sc2 {
    class IMBot : public sc2::Agent {
    private:
        //��Ԫ����
        UnitsVec m_units_vec;
        UnitsVec m_units_vec_self;
        UnitsVec m_units_vec_enemy;

        //
        IMVec m_IM_vec;
        IMptrList m_IMptr_list = {};
        GameSet m_game_set = GameSet();
        GameInfomation m_game_info;
        InfluenceMap m_IM = InfluenceMap(Neutral);
        InfluenceMap m_IM_self = InfluenceMap(Self);
        InfluenceMap m_IM_enemy = InfluenceMap(Enemy);
        IMPopVec m_IM_pop;

        //��־λ
        bool game_win_ = false;
        Tag targeted_enemy_tag_;
        //�ƶ���־λ
        bool move_back_;
        const Unit* targeted_enemy_;
        MapPoint backup_target_;
        MapPoint backup_start_;
        //��Ϸ״̬��־λ
        Game_Stage m_game_stage = Update_Flag;
        bool m_lock = false;

        //������
        Gameloop display_cnt = 0;
        Gameloop gameloop_cnt = 0;

    public:
        //��Ԫ��

        MyAlgorithm m_AL;


    public:
        // ��Ϸ����ִ��
        virtual void OnGameStart() final;
        // ��Ϸ��ִ֡�� Your bots OnStep function will be called each time the coordinator steps the simulation forward.
        virtual void OnStep() final;
        // ��λ���ݻ�ִ��
        virtual void OnUnitDestroyed(const Unit* unit) override;

        inline bool get_Game_Ended_() { return game_win_; }
        void set_Game_Ended_(sc2::GameResult game_result);
    public:
        //������Ϸ��Ϣ
        bool updateGameInfo(const UnitsVec& units);
        const HPself getHPSelf(const UnitsVec& units) const;
        const HPenemy getHPEnemy(const UnitsVec& units) const;
        bool updatePerGameLoop(const Gameloop loop_cnt);
        IMPopVec updateIMPop(const UnitsVec& units_vec_enemy);
        bool isNewPop(const GridPoint& grid_point, const std::vector<int>& vec);
        std::vector<int> findBeighborsIn2(const GridPoint& grid_point, arrint2D* arr_ptr);
        IMPopId getIMPop(const MapPoint& map_point);
        void writeIMptrList();
        MapPoint getCenterSelf(const UnitsVec& units);
    };


}

#endif // _IMBOT_H_