#ifndef _MICROTSP_H_
#define _MICROTSP_H_

#include <vector>
#include <sc2api/sc2_api.h>
#include <fstream>

const int StepSize = 9;
const int UnitSize = StepSize;
/**************************************/
typedef int IMPopId;
typedef sc2::Point2DI GridPoint;
typedef std::vector<GridPoint> IMPop;
IMPop IM_pop = {};

struct NodeUnit {
    int n_code;
    sc2::Unit::Alliance n_alliance;
    sc2::Tag n_tag;
    sc2::UnitTypeID n_type;
    IMPopId n_pop;
    sc2::Point3D n_pos;

    NodeUnit(int code, sc2::Unit::Alliance alliance, sc2::Tag tag, sc2::UnitTypeID type,IMPopId pop, sc2::Point3D pos = sc2::Point3D()) :
        n_code(code), n_alliance(alliance), n_tag(tag), n_type(type), n_pop(pop), n_pos(pos) {}
};

//�㷨����Ҫ����Ϸ��Ϣ
struct GameInfomation {
    std::vector<NodeUnit> info_vec_unit;
};
/**************************************/


class Micro_TSP {
private:
    //��Ԫ�������洢�������
    std::vector<NodeUnit> vec_TSP;
    //��Ԫ���ͱ�־λ
    bool unit_flag = false;

public:
    Micro_TSP() = default;
    ~Micro_TSP() = default;


    //�Ե�Ԫ����
    void encode_node(const GameInfomation& game_info);
    //���ʵ�Ԫ����
    std::vector<NodeUnit>& get_mineral_vec();

};


//������
void Micro_TSP::encode_node(const GameInfomation& game_info)
{
    for (int i = 0; i < game_info.info_vec_unit.size(); ++i) {
        if (game_info.info_vec_unit.at(i).n_alliance == Self) {
            NodeUnit node(0, sc2::Unit::Self, game_info.info_vec_unit.at(i).n_tag,
                game_info.info_vec_unit.at(i).n_type, game_info.info_vec_unit.at(i).n_pop, game_info.info_vec_unit.at(i).n_pos);
            vec_TSP.push_back(node);
            break;
        }
    }

    for (int i = 0, j = 0; i < game_info.info_vec_unit.size(); ++i) {
        if (game_info.info_vec_unit.at(i).n_alliance == Enemy) {
            ++j;
            NodeUnit node(j, sc2::Unit::Enemy, game_info.info_vec_unit.at(i).n_tag,
                game_info.info_vec_unit.at(i).n_type, game_info.info_vec_unit.at(i).n_pop);
            InfluenceMap temp;
            MapPoint point = temp.turnGridToMap(IM_pop.at(game_info.info_vec_unit.at(i).n_pop));
            vec_TSP.push_back(node);
        }
    }

    for (int i = 0; i < vec_TSP.size(); i++) {
        vec_TSP.at(i).n_code = i;
    }
}

std::vector<NodeUnit>& Micro_TSP::get_mineral_vec(){
    return this->vec_TSP;
}

#endif // _MICROTSP_H_