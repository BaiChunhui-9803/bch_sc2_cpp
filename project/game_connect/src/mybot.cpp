#include "sc2api/sc2_api.h"
#include "mybot.h"
#include <iostream>
#include <fstream>

const float kCameraWidth = 24.0f;
const int kFeatureLayerSize = 80;
const int kPixelDrawSize = 5;
const int kDrawSize = kFeatureLayerSize * kPixelDrawSize;

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
        if (save_state.isBlank()) {

        }
        else {
            //save_state.LoadState(save_state, *this, )
            game_load_flag_ = true;
        }
		// sc2::renderer::Initialize("Feature layers", 50, 50, kDrawSize, kDrawSize);
	}

	void MyConnectBot::OnStep() {

		const ObservationInterface* observation = Observation();
		ActionInterface* action = Actions();
		sc2::DebugInterface* debug = Debug();
		const SC2APIProtocol::Observation* observation_layer = Observation()->GetRawObservation();
        Units units_self = observation->GetUnits(Unit::Alliance::Self);
        Units units_enemy = observation->GetUnits(Unit::Alliance::Enemy);

        //if () {
        //    getGameInf();
        //}

        if (game_save_flag_) {
            save_state = save_state.SaveState(observation);
            std::cout << save_state;
            game_save_flag_ = false;
        }

        if (game_pause_flag_) {

            /*while (1) {
            }*/
        }
	}


    void sc2::MyConnectBot::getGameInf() {
        begin_selfHP = 0.0;
        begin_enemyHP = 0.0;
        sc2::Units units = Observation()->GetUnits();
        sc2::Units self_units = Observation()->GetUnits(Unit::Alliance::Self);
        sc2::Units enemy_units = Observation()->GetUnits(Unit::Alliance::Enemy);
        int num_units = units.size();
        int num_self_units = self_units.size();
        int num_enemy_units = enemy_units.size();
        for (int i = 0; i < self_units.size(); ++i) {
            begin_selfHP += self_units.at(i)->health_max;
        }
        for (int i = 0; i < enemy_units.size(); ++i) {
            begin_enemyHP += enemy_units.at(i)->health_max;
        }
        std::cout << "begin_selfHP:" << begin_selfHP << std::endl;
        std::cout << "begin_enemyHP:" << begin_enemyHP << std::endl;
    }

    double sc2::MyConnectBot::getScore() {
        //double max_selfHP = Observation();
        return 0.0;
    }

}

