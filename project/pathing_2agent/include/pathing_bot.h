#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2renderer/sc2_renderer.h"

#include "windows.h"
#include "Mineral_TSP.h"
#include "GA_TSP.h"

using namespace sc2;

const float kCameraWidth = 24.0f;
const int kFeatureLayerSize = 80;
const int kPixelDrawSize = 5;
const int kDrawSize = kFeatureLayerSize * kPixelDrawSize;

enum Game_Stage {
    //空环节
    Blank_Flag = 0,
    //节点更新环节
    Update_Flag = 1,
    //执行算法环节
    Algorithm_Flag = 2,
    //单元1移动环节
    Move_Flag = 3,
    ////单元2移动环节
    //Move_Flag2 = 4,
    ////单元1移动停止
    //MoveStop_Flag1 = 5,
    ////单元2移动停止
    //MoveStop_Flag2 = 6,
    //游戏结束环节
    End_Flag = 7
};


//友元类前向声明
class Mineral_TSP;
class GA_TSP;

void DrawFeatureLayer1BPP(const SC2APIProtocol::ImageData& image_data, int off_x, int off_y) {
    assert(image_data.bits_per_pixel() == 1);
    int width = image_data.size().x();
    int height = image_data.size().y();
    sc2::renderer::Matrix1BPP(image_data.data().c_str(), width, height, off_x, off_y, kPixelDrawSize, kPixelDrawSize);
}

void DrawFeatureLayerUnits8BPP(const SC2APIProtocol::ImageData& image_data, int off_x, int off_y) {
    assert(image_data.bits_per_pixel() == 8);
    int width = image_data.size().x();
    int height = image_data.size().y();
    sc2::renderer::Matrix8BPPPlayers(image_data.data().c_str(), width, height, off_x, off_y, kPixelDrawSize, kPixelDrawSize);
}

void DrawFeatureLayerHeightMap8BPP(const SC2APIProtocol::ImageData& image_data, int off_x, int off_y) {
    assert(image_data.bits_per_pixel() == 8);
    int width = image_data.size().x();
    int height = image_data.size().y();
    sc2::renderer::Matrix8BPPHeightMap(image_data.data().c_str(), width, height, off_x, off_y, kPixelDrawSize, kPixelDrawSize);
}

class PathingBot :public Agent {
private:
    int i1 = 0, i2 = UnitSize - 1;
    Game_Stage m_game_stage = Update_Flag;
    sc2::Tag tag_begin1;
    sc2::Tag tag_begin2;
public:
    //友元类
    friend class Mineral_TSP;
    friend class GA_TSP;
    //Mineral_TSP方法的执行器，供Bot使用
    Mineral_TSP m_Actuator;
    //GA_TSP方法的执行器，供Bot使用
    GA_TSP m_GA;
    //通过算法得到的最优部署解
    std::vector<Node_TSP> m_best_solution;
    bool flag = false;
    bool is_end(const sc2::ObservationInterface* observation);
    int mineral_now = 0;
    bool SetBegin_Flag = false;
    bool Move1_Flag = false;
    bool Move2_Flag = false;
    void PathingBot::draw_OnMATLAB();

public:
    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
        sc2::renderer::Initialize("Feature layers", 50, 50, kDrawSize, kDrawSize);
    }

    virtual void OnStep() final {
        //观测器
        const sc2::ObservationInterface* observation = Observation();
        //控制器
        sc2::ActionInterface* action = Actions();
        //单元容器
        sc2::Units units_vec = observation->GetUnits();
        //图层控制器（UI:鼠标选择，相机移动？）
        sc2::ActionFeatureLayerInterface* featurelayeraction = ActionsFeatureLayer();
        //输出窗口
        sc2::DebugInterface* debug = Debug();
        sc2::Point2D debug_point(0,0.2);
        sc2::Point2D debug_point_delta_x(0.01, 0);
        sc2::Point2D debug_point_delta_y(0, 0.01);
        debug->DebugTextOut("MAP:CollectMineralShards_12000s", debug_point, Colors::White);
        debug->DebugTextOut("PROBLEM:2Agent 20Node TSP", debug_point + debug_point_delta_y * 2, Colors::White);
        debug->DebugTextOut("ALGORITHM:GA", debug_point + debug_point_delta_y * 4, Colors::White);
        //debug->DebugBoxOut(Point3D(20, 20, 20), Point3D(30, 30, 0));


        //绘制特征层
        const SC2APIProtocol::Observation* observation_layer = Observation()->GetRawObservation();

        const SC2APIProtocol::FeatureLayers& m = observation_layer->feature_layer_data().renders();
        DrawFeatureLayerUnits8BPP(m.unit_density(), 0, 0);
        DrawFeatureLayer1BPP(m.selected(), kDrawSize, 0);

        //const SC2APIProtocol::FeatureLayersMinimap& mi = observation_layer->feature_layer_data().minimap_renders();
        //DrawFeatureLayerHeightMap8BPP(mi.height_map(), 0, kDrawSize);
        //DrawFeatureLayer1BPP(mi.camera(), kDrawSize, kDrawSize);

        sc2::renderer::Render();
        //绘制特征层


        mineral_now = observation->GetMinerals();

        if (mineral_max < observation->GetMinerals()) {
            mineral_max = observation->GetMinerals();
            std::cout << "GameLoop:" << observation->GetGameLoop() << '\t';
            std::cout << "Minerals:" << observation->GetMinerals() << std::endl;
            debug->DebugTextOut("GameLoop:" + std::to_string(observation->GetGameLoop()));
            /*std::ofstream fout;
            fout.open("D:/bch_sc2_OFEC/sc2api/project/pathing_2agent/datafile/compare_data/compare_pathing_2agent.txt", std::ios::out | std::ios::app);
            fout << observation->GetGameLoop() << '\t' << observation->GetMinerals() << std::endl;*/

            //debug->SendDebug();
        }

        if (m_game_stage == Update_Flag) {
            if (m_Actuator.update_Unit(units_vec)) {
                m_game_stage = Algorithm_Flag;
                std::cout << "结点信息更新完毕，进行优化" << std::endl;
                action->SendChat("Updating done, now perform optimization.");
            }
        }

        if (m_game_stage == Algorithm_Flag) {
            m_GA.initialize_Algorithm(m_Actuator.get_mineral_vec());
            m_best_solution = m_GA.GA();
            action->SendChat("Optimization done, now perform solution.");
            draw_OnMATLAB();
            m_game_stage = Move_Flag;
            action->SendChat("Start moving to collect.");
        }
        std::vector<sc2::Point2D> point_vec;
        int i = 0;
        for (auto it : m_best_solution) {
            sc2::Point2D point(it.m_x, it.m_y);
            point_vec.push_back(point);
            if (i < m_best_solution.size() - 1 && i != (m_best_solution.size() / 2 - 1)) {
                Point3D p0(m_best_solution.at(i).m_x, m_best_solution.at(i).m_y, m_best_solution.at(i).m_z);
                Point3D p1(m_best_solution.at(i + 1).m_x, m_best_solution.at(i + 1).m_y, m_best_solution.at(i + 1).m_z);
                debug->DebugLineOut(p0, p1, Colors::Green);
            }
            i++;
        }

        if (flag == true && mineral_now % ((UnitSize - 2) * 100) != 0) {
            flag = false;
        }

        if (!point_vec.empty() && !SetBegin_Flag) {
            for (const auto&u : units_vec) {
                if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE) {
                    if (Distance2D(u->pos, point_vec.at(0)) < 0.5f) tag_begin1 = u->tag;
                    else tag_begin2 = u->tag;
                }
            }
            SetBegin_Flag = true;
        }

        sc2::Point3D begin1_pos(0, 0, 0);
        sc2::Point3D begin2_pos(0, 0, 0);
        if (!point_vec.empty()) {
            for (const auto&u : units_vec) {
                if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && u->tag == tag_begin1) {
                    begin1_pos = u->pos;
                    sc2::Point2D point_begin1(u->pos.x, u->pos.y);
                    debug->DebugTextOut("begin1 coord:(" + std::to_string(u->pos.x) + "," + std::to_string(u->pos.y) + "," + std::to_string(u->pos.z) + ")", debug_point + debug_point_delta_y * 6, Colors::Yellow);
                    if (m_game_stage == Move_Flag && Move1_Flag) {
                        //action->UnitCommand(u, ABILITY_ID::PATROL, point_vec.at(i1 + 1));
                        action->UnitCommand(u, ABILITY_ID::MOVE, point_vec.at(i1 + 1));
                        i1++;
                        m_game_stage = Blank_Flag;
                        Move1_Flag = false;
                    }
                    else {
                        if (Distance2D(point_begin1, point_vec.at(i1)) < 1.5f) {
                            m_game_stage = Move_Flag;
                            Move1_Flag = true;
                        }
                    }
                    if (flag == false && observation->GetMinerals() != 0 && observation->GetMinerals() % 2000 == 0) {
                        action->UnitCommand(u, ABILITY_ID::STOP);
                        for (int i = 0; i < units_vec.size(); i++) {
                            if (units_vec.at(i)->tag == tag_begin2) {
                                action->UnitCommand(units_vec.at(i), ABILITY_ID::STOP);
                                break;
                            }
                        }
                        point_vec.clear();
                        m_Actuator.get_mineral_vec().clear();
                        m_GA.~GA_TSP();
                        m_game_stage = Update_Flag;
                        flag = true;
                        mineral_now = observation->GetMinerals();
                        i1 = 0;
                        i2 = UnitSize - 1;
                        SetBegin_Flag = false;
                        Move1_Flag = false;
                        Move2_Flag = false;
                        break;
                    }
                }

                if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && u->tag == tag_begin2) {
                    begin2_pos = u->pos;
                    sc2::Point2D point_begin2(u->pos.x, u->pos.y);
                    debug->DebugTextOut("begin2 coord:(" + std::to_string(u->pos.x) + "," + std::to_string(u->pos.y) + "," + std::to_string(u->pos.z) + ")", debug_point + debug_point_delta_y * 8, Colors::Yellow);
                    if (m_game_stage == Move_Flag && Move2_Flag) {
                        //action->UnitCommand(u, ABILITY_ID::PATROL, point_vec.at(i2 - 1));
                        action->UnitCommand(u, ABILITY_ID::MOVE, point_vec.at(i2 - 1));
                        i2--;
                        m_game_stage = Blank_Flag;
                        Move2_Flag = false;
                    }
                    else {
                        if (Distance2D(point_begin2, point_vec.at(i2)) < 1.5f) {
                            m_game_stage = Move_Flag;
                            Move2_Flag = true;
                        }
                    }
                    if (flag == false && observation->GetMinerals() != 0 && observation->GetMinerals() % 2000 == 0) {
                        action->UnitCommand(u, ABILITY_ID::STOP);
                        for (int i = 0; i < units_vec.size(); i++) {
                            if (units_vec.at(i)->tag == tag_begin1) {
                                action->UnitCommand(units_vec.at(i), ABILITY_ID::STOP);
                                break;
                            }
                        }
                        point_vec.clear();
                        m_Actuator.get_mineral_vec().clear();
                        m_GA.~GA_TSP();
                        m_game_stage = Update_Flag;
                        flag = true;
                        mineral_now = observation->GetMinerals();
                        i1 = 0;
                        i2 = UnitSize - 1;
                        SetBegin_Flag = false;
                        Move1_Flag = false;
                        Move2_Flag = false;
                        break;
                    }
                }
            }
        }
        //debug->DebugLineOut(begin1_pos, begin2_pos);
        //debug->DebugSphereOut(begin1_pos, 0.75);
        //debug->DebugSphereOut(begin2_pos, 0.75);
        debug->SendDebug();
        //featurelayeraction->Select(Point2DI(20, 20), Point2DI(800, 800));
        //featurelayeraction->SendActions();
    }
    int mineral_max = 0;
};

bool PathingBot::is_end(const sc2::ObservationInterface* observation) {
    if (observation->GetMinerals() != 0 && observation->GetMinerals() % 2000 == 0) {
        return true;
    }
    else {
        return false;
    }
}

void PathingBot::draw_OnMATLAB() {
    char n;
    POINT p;
    //GetCursorPos(&p);
    //SetCursorPos(2416, 84);//移动到指定位置
    //mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);//按下左键
    //mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);//松开左键
}