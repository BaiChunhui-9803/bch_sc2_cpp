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

//解的表示
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

//算法所需要的游戏信息
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
    //存放解的容器
    std::vector<MAP_MYSOL> m_sol_vec;
    void map_insert_(MAP_MYSOL& sol_map, const ValueMySol& sol_value);
    //栅格地图
    int GridMap[GridSize][GridSize] = {};
    //栅格地图的概率分布
    double GridMap_P[GridSize][GridSize] = {};
    //栅格地图的选择概率
    double GridMap_SP[GridSize][GridSize] = {};

public:

    GA_micro() = default;
    ~GA_micro() = default;

    //算法初始化
    void initialize_Algorithm(const GameInfomation& game_info);
    //获取随机解
    MAP_MYSOL get_random_solution();
    //获取一组随机解的vector
    std::vector<MAP_MYSOL> get_random_solution_vec();
    //算法主体部分
    std::vector<MAP_MYSOL> Algoritrm();
    //势场地图栅格化
    void grid_map();
    //给定Point坐标，将其转为9x9矩阵中的点
    GridPoint map_to_grid(sc2::Point3D map_point);
    //给定9x9矩阵中的点，将其转为Point坐标
    sc2::Point3D grid_to_map(GridPoint grid_point);
    //给定9x9矩阵中的点，依据概率选择另一个比其低的相邻的点
    GridPoint select_grid(GridPoint grid_point);
    //给定9x9矩阵中的点，获取其8个方向相邻的点
    std::vector<GridPoint> find_beighbors(GridPoint grid_point);
    //从vector中按照GridMap的值抽签
    GridPoint Lottery_from_vector(const std::vector<GridPoint>& vec);
};
