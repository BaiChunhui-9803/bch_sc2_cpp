#pragma once

#include <sc2api/sc2_api.h>

#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <random>
#include <map>

#define MapSize 100
#define MoveSize 18
#define GridSize 9
#define SolutionLength 10

using namespace sc2;
typedef uint32_t Gameloop;

//��ı�ʾ
typedef std::map<Gameloop, std::pair<sc2::ABILITY_ID, sc2::Point3D>> MAP_MYSOL;
struct ValueMySol {
    Gameloop v_gameloop;
    sc2::ABILITY_ID v_ability_id;
    sc2::Point3D v_point;

    ValueMySol(Gameloop gameloop, sc2::ABILITY_ID ability_id, sc2::Point3D point) :v_gameloop(gameloop), v_ability_id(ability_id), v_point(point) {}
};

struct NodeUnit {
    sc2::Unit::Alliance n_alliance;
    sc2::Tag n_tag;
    sc2::UnitTypeID n_type;
    sc2::Point3D n_pos;
};

//�㷨����Ҫ����Ϸ��Ϣ
struct GameInfomation {
    std::vector<NodeUnit> vec_unit;

};

struct MapPoint {
    double XPoint;
    double YPoint;
};

struct GridPoint {
    int XPoint;
    int YPoint;

    GridPoint(int x, int y) :XPoint(x), YPoint(y) {}
};

class GA_micro {
private:
    //��Ž������
    std::vector<MAP_MYSOL> m_sol_vec;
    void map_insert_(MAP_MYSOL& sol_map, const ValueMySol& sol_value);
    //դ���ͼ
    int GridMap[GridSize][GridSize] = {};
    //դ���ͼ�ĸ��ʷֲ�
    double GridMap_P[GridSize][GridSize] = {};
    //դ���ͼ��ѡ�����
    double GridMap_SP[GridSize][GridSize] = {};

public:

    GA_micro() = default;
    ~GA_micro() = default;

    //�㷨��ʼ��
    void initialize_Algorithm(const GameInfomation& game_info);
    //��ȡ�����
    MAP_MYSOL get_random_solution();
    //��ȡһ��������vector
    std::vector<MAP_MYSOL> get_random_solution_vec();
    //�㷨���岿��
    std::vector<MAP_MYSOL> Algoritrm();
    //�Ƴ���ͼդ��
    void grid_map();
    //����Point���꣬����תΪ9x9�����еĵ�
    GridPoint map_to_grid(sc2::Point3D map_point);
    //����9x9�����еĵ㣬����תΪPoint����
    sc2::Point3D grid_to_map(GridPoint grid_point);
    //����9x9�����еĵ㣬���ݸ���ѡ����һ������͵����ڵĵ�
    GridPoint select_grid(GridPoint grid_point);
    //����9x9�����еĵ㣬��ȡ��8���������ڵĵ�
    std::vector<GridPoint> find_beighbors(GridPoint grid_point);
    //��vector�а���GridMap��ֵ��ǩ
    GridPoint Lottery_from_vector(const std::vector<GridPoint>& vec);
};
