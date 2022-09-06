/*
done
*/
//ctime务必在前
#include <ctime>
//#include <sc2api/sc2_api.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "sc2utils/sc2_manage_process.h"

#include "Mineral_TSP.h"
#include "GA_TSP.h"
#include "pathing_bot.h"

using namespace std;
int main(int argc, char* argv[]) {

	sc2::Coordinator coordinator;
	//coordinator.LoadSettings(argc, argv);
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}

	coordinator.SetFeatureLayers(sc2::FeatureLayerSettings(24.0f, 64, 64, 64, 64));


	coordinator.SetWindowLocation(800, 100);
	coordinator.SetRealtime(false);
	coordinator.SetStepSize(1);
	coordinator.SetMultithreaded(true);
	//coordinator.SetWindowSize(100, 100);
	//coordinator.SetRender(sc2::RenderSettings(100,100,100,100));
	//coordinator.SetFeatureLayers(FeatureLayerSettings(24.0f, 80, 80, 80, 80));


	PathingBot bot;
	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Terran, &bot),
		sc2::CreateComputer(sc2::Race::Zerg)
		});

	coordinator.LaunchStarcraft();
	const char* kmap = "mini_games/CollectMineralShards_1800s.SC2Map";
	coordinator.StartGame(kmap);

	while (coordinator.Update()) {
		if (sc2::PollKeyPress()) {
			break;
		}
	}

	return 0;
}