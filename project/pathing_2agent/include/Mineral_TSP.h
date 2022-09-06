#pragma once

#include <vector>
#include <sc2api/sc2_api.h>
#include <fstream>

const int UnitSize = 22;

enum Node_Type {
    //资源结点
    Mineral = 0,
    //遍历结点的开始
    Begin1 = 1,
    //空结点，不进行遍历
    Begin2 = 2
};

struct Node_TSP {
    Node_Type m_type;
    int m_mineral_id;
    int m_mineral_code;
    float m_x;
    float m_y;
    //游戏坐标z轴，不使用
    float m_z;

    Node_TSP(Node_Type type, int id, int code, float x, float y, float z) :
        m_type(type), m_mineral_id(id), m_mineral_code(code), m_x(x), m_y(y), m_z(z) {}
};


class Mineral_TSP {
private:
    //单元容器，存储各个结点
    std::vector<Node_TSP> mineral_vec;
    //单元类型标志位
    bool unit_flag = false;

public:
    Mineral_TSP() = default;
    ~Mineral_TSP() = default;


    //从游戏读取单元数据，存储至mineral_vec
    bool update_Unit(sc2::Units units);
    //判断单元是否已经添加
    bool is_uniqueNode(const sc2::Unit* unit_pos);
    //对单元编码
    void encode_node(const sc2::Unit* unit_pos);
    //访问单元容器
    std::vector<Node_TSP>& get_mineral_vec();
};

//更新单元信息到文件unit_data.txt
bool Mineral_TSP::update_Unit(sc2::Units units) {

    std::ofstream fout;
    //fout.open("test.txt", std::ios::app);
    fout.open("D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/unit_data.txt");

    for (int i = 0; i < UnitSize; i++) {

        if (is_uniqueNode(units.at(i))) {
            encode_node(units.at(i));
        }
    }

    for (auto it : mineral_vec) {
        fout << it.m_type << '\t' << it.m_mineral_id << '\t' << it.m_mineral_code << '\t' << it.m_x << '\t' << it.m_y  << std::endl;
    }

    fout.close();
    //std::cout << "22个结点的数据已经输出至text.txt" << std::endl;
    return true;
}

//判断结点是否唯一
bool Mineral_TSP::is_uniqueNode(const sc2::Unit * unit_pos)
{

    if (mineral_vec.size() == 0) {
        return true;
    }
    for (auto it : mineral_vec) {
        if (it.m_x == unit_pos->pos.x && it.m_y == unit_pos->pos.y &&it.m_z == unit_pos->pos.z) {
            return false;
        }
    }

    return true;

}

//结点编码，保证Begin和Blank在首尾
void Mineral_TSP::encode_node(const sc2::Unit * unit_pos)
{
    Node_TSP node(Mineral, 0, 0, unit_pos->pos.x, unit_pos->pos.y, unit_pos->pos.z);
    if (unit_pos->unit_type == 1680) {
        node.m_type = Mineral;
    }

    else if (unit_pos->unit_type == 48 && unit_flag == false) {
        node.m_type = Begin1;
        unit_flag = true;
    }

    else if (unit_pos->unit_type == 48 && unit_flag == true) {
        node.m_type = Begin2;
        unit_flag = false;
    }

    mineral_vec.push_back(node);

    for (int i = 0; i < mineral_vec.size(); i++) {
        mineral_vec.at(i).m_mineral_id = i;
    }
}

std::vector<Node_TSP>& Mineral_TSP::get_mineral_vec(){
    return this->mineral_vec;
}