#ifndef _INFLUENCEMAP_H_
#define _INFLUENCEMAP_H_
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2renderer/sc2_renderer.h"
#include "gnuplot-iostream.h"
/***********************/
//#define _DRAW_
//#define _AUTO_CLOSE_PLOT
/***********************/

#ifdef _WIN32
#include "windows.h"
static int flagPressQ = 0;
#endif


//离散势力图的长度/宽度
#define MAP_GRID_SIZE 50
//实际势力图的长度
#define MAP_X_LENGTH 50
//实际势力图的宽度
#define MAP_Y_LENGTH 50
//势力范围大小
#define Rule_Size 4

//UnitsVec 单元容器，包含敌方、友方
typedef sc2::Units UnitsVec;
//MapPoint 实际势力图坐标
typedef sc2::Point2D MapPoint;
//GridPoint 离散势力图坐标，映射在离散矩阵里
typedef sc2::Point2DI GridPoint;
//arrint2D* 二维数组指针
typedef const int arrint2D[MAP_GRID_SIZE];
typedef std::vector<std::pair<GridPoint, size_t>> IMPopVec;

//读写文件路径
static const std::string fOutPathIMarr = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr.txt";
static const std::string fOutPathIMarr3r = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr_3row.txt";
static const std::string fOutPathIMarrSelf = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr_self.txt";
static const std::string fOutPathIMarrSelf3r = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr_self_3row.txt";
static const std::string fOutPathIMarrEnemy = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr_enemy.txt";
static const std::string fOutPathIMarrEnemy3r = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr_enemy_3row.txt";

//势力图的敌我关系
enum MapAlliance {
    Self = 1,
    Enemy = 4,
    Neutral = 3,
};

//势力范围形状
enum InfluenceShape {
    Square = 1,
    Circle = 2,
};

enum Direction {
    CSYS1 = 1,
    CSYS2 = 2,
    CSYS3 = 3,
    CSYS4 = 4,
};

struct InfluenceRule {
    InfluenceShape m_shape = Square;
    unsigned int m_range = Rule_Size;
    //由于递增原因，势力增长为16、9=5+3+1、4=3+1、1
    int m_self_level[Rule_Size] = { 16, 5, 3, 1 };
    int m_enemy_level[Rule_Size] = { -16, -5, -3, -1 };
};

//势力图的关键点坐标
class MapPosition {
private:
    MapPoint m_center;
    MapPoint m_top_left;
    MapPoint m_top_right;
    MapPoint m_bottom_left;
    MapPoint m_bottom_right;

public:
    MapPosition() :
        m_center(MapPoint(64.0f, 64.0f)),
        m_top_left(MapPoint(64.0f - MAP_X_LENGTH / 2.0, 64.0f + MAP_Y_LENGTH / 2.0)),
        m_top_right(MapPoint(64.0f + MAP_X_LENGTH / 2.0, 64.0f + MAP_Y_LENGTH / 2.0)),
        m_bottom_left(MapPoint(64.0f - MAP_X_LENGTH / 2.0, 64.0f - MAP_Y_LENGTH / 2.0)),
        m_bottom_right(MapPoint(64.0f + MAP_X_LENGTH / 2.0, 64.0f - MAP_Y_LENGTH / 2.0)) {};
    ~MapPosition() = default;
    MapPosition(UnitsVec& unit_vector);
    MapPoint getCenter() const { return m_center; }
    MapPoint getTopLeft() const { return m_top_left; }
    MapPoint getTopRight() const { return m_top_right; }
    MapPoint getBottomLeft() const { return m_bottom_left; }
    MapPoint getBottomRight() const { return m_bottom_right; }
};



//势力图类
class InfluenceMap {
    friend class MapPosition;
public:
    //势力图的位置
    MapPosition m_map_position;
    //势力图的敌我关系
    MapAlliance m_map_alliance;
    //势力图的离散矩阵
    int m_map_arr[MAP_GRID_SIZE][MAP_GRID_SIZE] = {};



public:
    InfluenceMap();
    InfluenceMap(MapAlliance map_alliance);
    InfluenceMap(UnitsVec& unit_vector, MapAlliance map_alliance);
    ~InfluenceMap() = default;
    //InfluenceMap(const InfluenceMap& IM);
    //InfluenceMap& operator=(const InfluenceMap& IM);
    MapPosition getMapPosition() const { return m_map_position; }
    MapAlliance getMapAlliance() const { return m_map_alliance; }
    arrint2D* getMapArray() const { return m_map_arr; }


    //给定MapPoint坐标，将其转为矩阵中的点
    GridPoint turnMapToGrid(const MapPoint& map_point);
    //给定GridPoint坐标，将其转为实际势力图中的点
    MapPoint turnGridToMap(const GridPoint& grid_point);
    //根据unit_vector更新势力图中的势力值
    void updateIMValue(const UnitsVec& unit_vector);
    //将IMarr写入文件中
    void writeIMarrToFile(const std::string foutPath, const std::string fout3rPath);
    ////给定units_enemy，将IM_neutral->IM_self
    //bool turnIMNtoIMS(InfluenceMap& IM, const UnitsVec& units_enemy);
    ////给定units_self，将IM_self->IM_neutral
    //bool turnIMNtoIME(const UnitsVec& units_self);
    //给定一组units，计算中心位置的pos
    MapPoint getCenterMapPoint(const std::vector<MapPoint>& vec_mappoint);
    //计算两点之间的距离
    float calculateDistance(const MapPoint& point_a, const MapPoint& point_b);
    //计算两点(点于群)之间的带权距离
    float calculateWeightDistance(const IMPopVec& popvec, const int index, const MapPoint& point_a, const MapPoint& point_b);
    //计算两点(点于点)之间的引力
    //float calculateGravitation(const IMPopVec& popvec, const int index, const MapPoint& point_a, const MapPoint& point_b);
    //给定MapPoint点A,B,判断B在方位A的方位
    Direction getDirection2P(const MapPoint& point_a, const MapPoint& point_b);

private:
    void update_(const sc2::Unit* const unit);
    void update_beighbors(GridPoint center, sc2::Unit::Alliance alliance, int level, InfluenceRule rule = InfluenceRule());
};

#ifdef _DRAW_
//调用gnuplot可视化IMarr
void displayIMarr();

#ifdef _WIN32
#ifdef _AUTO_CLOSE_PLOT
//调用windows.h快捷键关闭plot
void pressQ();
#endif
#endif
#endif

#endif // _INFLUENCEMAP_H_