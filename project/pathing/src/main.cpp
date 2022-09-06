/*
done
*/

#include <sc2api/sc2_api.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>

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

	coordinator.SetWindowLocation(800, 100);
	coordinator.SetRealtime(false);

	//PathingBot bot;
	RandBotAgent bot;
	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Terran, &bot),
		sc2::CreateComputer(sc2::Race::Zerg)
		});

	coordinator.LaunchStarcraft();
	const char* kmap = "mini_games/CollectMineralShards_1800s.SC2Map";
	coordinator.StartGame(kmap);

	while (coordinator.Update()) {

	}

	return 0;
}