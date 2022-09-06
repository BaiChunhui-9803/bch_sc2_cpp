#define _MYALGORITHM_H_
#ifdef _MYALGORITHM_H_

#include <sc2api/sc2_api.h>
#include <vector>

#include "InfluenceMap.h"

using namespace sc2;
typedef uint32_t Gameloop;
typedef int IMPopId;
typedef std::vector<std::pair<GridPoint, size_t>> IMPopVec;

//��ı�ʾ,һ������Gameloop��־�Ķ���/λ������
typedef std::map<Gameloop, std::pair<sc2::ABILITY_ID, sc2::Point3D>> MySoL;
//��������ı�ʾ
typedef std::vector<MySoL> VecMySoL;

struct ValueMySol {
    Gameloop v_gameloop;
    sc2::ABILITY_ID v_ability_id;
    sc2::Point3D v_point;

    ValueMySol(Gameloop gameloop, sc2::ABILITY_ID ability_id, sc2::Point3D point) :
        v_gameloop(gameloop),
        v_ability_id(ability_id),
        v_point(point) {}
};

struct NodeUnit {
    sc2::Unit::Alliance n_alliance;
    sc2::Tag n_tag;
    sc2::UnitTypeID n_type;
    sc2::Point3D n_pos;
    IMPopId n_pop;
};

//�㷨����Ҫ����Ϸ��Ϣ
struct GameInfomation {
    std::vector<NodeUnit> info_vec_unit;
};

class MyAlgorithm {
private:
    //��Ž������
    VecMySoL myal_vec_sol;
    //��Ϸ��Ϣ(��IMBot����)
    GameInfomation myal_game_info;
    //IMpop(��IMBot����)
    IMPopVec myal_IM_pop;
    //IM(ִ����)
    InfluenceMap myal_IM = InfluenceMap(Neutral);
    //SelfUnits������λ��
    MapPoint myal_center_self;
    //EnemyUnits������λ��
    MapPoint myal_center_enemy;

public:

    //�㷨��ʼ��
    void initializeAlgorithm(const GameInfomation& game_info, const IMPopVec& IM_pop);
    //�㷨����
    bool updateAlgorithm(const GameInfomation& game_info, const IMPopVec& IM_pop);
    //����myal_center_self
    MapPoint getCenterSelf();
    //����myal_center_enemy
    MapPoint getCenterEnemy();
    //Ѱ�Ҵ�Ȩ��������ĵ�
    sc2::Tag findNearsetPoint();
    //�㷨����IM_pop����һ����

    //����kite������
    MapPoint getKiteMapPoint(const int index);
    //����tag,���������ڵ�Ⱥ
    int getPopIndex(const sc2::Tag tag);
    //����index,��λ,����Ⱥ�ı�Ե
};





#endif // _MYALGORITHM_H_