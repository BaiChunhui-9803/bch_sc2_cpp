#include "sc2api/sc2_api.h"
#include "mybot.h"
#include <iostream>
#include <fstream>

const float kCameraWidth = 24.0f;
const int kFeatureLayerSize = 80;
const int kPixelDrawSize = 5;
const int kDrawSize = kFeatureLayerSize * kPixelDrawSize;

const size_t selfN = 10;

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

namespace sc2 {

	void MyConnectBot::OnGameStart() {
        game_pause_flag_ = false;
        //if (save_state.isBlank()) {
        //    //setGameInf()
        //}
        //else {
        //    //save_state.LoadState(save_state, *this, )
        //    game_load_flag_ = true;
        //    setGameInf(save_state);
        //}
		// sc2::renderer::Initialize("Feature layers", 50, 50, kDrawSize, kDrawSize);
	}

	void MyConnectBot::OnStep() {

		const ObservationInterface* observation = Observation();
		ActionInterface* action = Actions();
		sc2::DebugInterface* debug = Debug();
		const SC2APIProtocol::Observation* observation_layer = Observation()->GetRawObservation();
        Units units_self = observation->GetUnits(Unit::Alliance::Self);
        Units units_enemy = observation->GetUnits(Unit::Alliance::Enemy);

        if (!game_stop_observe_flag_) {
            pushObservedUnits(observation);
            setGameInf(observed_units);
        }

        if (game_load_finish_flag_) {
            Units units_self = observation->GetUnits(Unit::Alliance::Self);
            for (auto& u : units_self) {
                switch (static_cast<UNIT_TYPEID>(u->unit_type)) {
                case UNIT_TYPEID::TERRAN_MARINE: {
                    action->UnitCommand(u, ABILITY_ID::SMART, Point2D(60, 60));
                    break;
                }
                default: {
                    break;
                }
                }
            }
        }

        debug->DebugMoveCamera(getCenterPos(units_self));
        debug->SendDebug();

        if (game_save_flag_) {
            save_state = save_state.SaveState(observation);
            //std::cout << save_state;
            game_save_flag_ = false;
        }

        //if (game_load_flag_) {
        //    setGameInf(save_state);
        //    game_load_flag_ = false;
        //}

        //showGameInf();

        if (game_pause_flag_) {
            m_scorer = getScore();
            game_pause_finish_flag_ = true;
        }
	}

    void sc2::MyConnectBot::pushObservedUnits(const ObservationInterface*& ob) {
        sc2::Units units = Observation()->GetUnits();
        for (auto& u : units) {
            if (observed_units.find(u->tag) == observed_units.end()) {
                observed_units.insert(std::pair<sc2::Tag, sc2::Unit>(u->tag, *u));
                //std::cout << u->tag << std::endl;
            }
        }
    }

    void sc2::MyConnectBot::setGameInf(const std::map<sc2::Tag, sc2::Unit>& observed) {
        begin_selfHP = 0.0;
        begin_enemyHP = 0.0;
        begin_selfN = 0;
        begin_enemyN = 0;
        for (std::map<sc2::Tag, sc2::Unit>::reverse_iterator iter = observed_units.rbegin(); iter != observed_units.rend(); ++iter) {
            if (iter->second.owner == 1) {
                ++begin_selfN;
                begin_selfHP += iter->second.health_max;
            }
            else if (iter->second.owner == 2) {
                ++begin_enemyN;
                begin_enemyHP += iter->second.health_max;
            }
        }
    }

    void sc2::MyConnectBot::setGameInf(const State& state) {
        begin_selfHP = 0.0;
        begin_enemyHP = 0.0;
        begin_selfN = 0;
        begin_enemyN = 0;
        for (int i = 0; i < state.m_units_state.size(); ++i) {
            if (state.m_units_state.at(i).m_player_id == 1) {
                begin_selfHP += state.m_units_state.at(i).m_life;
                ++begin_selfN;
            }
            else if (state.m_units_state.at(i).m_player_id == 2) {
                begin_enemyHP += state.m_units_state.at(i).m_life;
                ++begin_enemyN;
            }
        }
    }

    void sc2::MyConnectBot::setGameInf(double bsHP, double beHP, size_t bsN, size_t beN) {
        begin_selfHP = bsHP;
        begin_enemyHP = beHP;
        begin_selfN = bsN;
        begin_enemyN = beN;
    }

    void sc2::MyConnectBot::getGameInf() {
        begin_selfHP = 0.0;
        begin_enemyHP = 0.0;
        begin_selfN = 0;
        begin_enemyN = 0;
        sc2::Units units = Observation()->GetUnits();
        sc2::Units self_units = Observation()->GetUnits(Unit::Alliance::Self);
        sc2::Units enemy_units = Observation()->GetUnits(Unit::Alliance::Enemy);
        int num_units = units.size();
        begin_selfN = self_units.size();
        begin_enemyN = enemy_units.size();
        for (int i = 0; i < begin_selfN; ++i) {
            begin_selfHP += self_units.at(i)->health_max;
        }
        for (int i = 0; i < begin_enemyN; ++i) {
            begin_enemyHP += enemy_units.at(i)->health_max;
        }
        std::cout << "begin_selfN:" << begin_selfN << std::endl;
        std::cout << "begin_enemyN:" << begin_enemyN << std::endl;
        std::cout << "begin_selfHP:" << begin_selfHP << std::endl;
        std::cout << "begin_enemyHP:" << begin_enemyHP << std::endl;
    }

    void sc2::MyConnectBot::showGameInf() {

        std::cout << "begin_selfN:" << begin_selfN << std::endl;
        std::cout << "begin_enemyN:" << begin_enemyN << std::endl;
        std::cout<< "begin_selfHP:" << begin_selfHP << std::endl;
        std::cout << "begin_enemyHP:" << begin_enemyHP << std::endl;
    }

    MyScore sc2::MyConnectBot::getScore() {
        size_t current_selfN= 0;
        size_t current_enemyN = 0;
        double current_selfHP = 0.0;
        double current_enemyHP = 0.0;

        for (auto& u : Observation()->GetUnits()) {
            if (u->owner == 1) {
                ++current_selfN;
                current_selfHP += u->health;
            }
            else if (u->owner == 2) {
                ++current_enemyN;
                current_enemyHP += u->health;
            }
        }
        return MyScore((begin_enemyHP - current_enemyHP),
            (begin_selfHP - current_selfHP),
            (begin_enemyHP - current_enemyHP) - (begin_selfHP - current_selfHP)
        );
    }

    Point2D sc2::MyConnectBot::getCenterPos(const Units& units) {
        Point2D center_pos;
        for (auto& u : units) {
            center_pos += Point2D(u->pos);
        }
        return center_pos / units.size();
    }


}

