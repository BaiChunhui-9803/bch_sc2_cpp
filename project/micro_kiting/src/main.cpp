#include "sc2api/sc2_api.h"
#include "MicroKitingBot.h"
#include "MicroAgentBot.h"

#include "sc2utils/sc2_arg_parser.h"
#include "sc2utils/sc2_manage_process.h"


//*************************************************************************************************
int main(int argc, char* argv[]) {
	sc2::Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	//coordinator.SetWindowLocation(800, 100);
	coordinator.SetRealtime(true);

	//����Զ���bot
	MicroKitingBot bot;
	//MicroAgentBot bot;

	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Terran, &bot),
		CreateComputer(sc2::Race::Zerg,
				sc2::Difficulty::CheatInsane,
				sc2::AIBuild::Rush,
				"CheatInsane"
				),
		});


	// ������Ϸ
	coordinator.LaunchStarcraft();

	// ���ص�ͼ
	//coordinator.StartGame(sc2::kMapMarineMicro);
	//coordinator.StartGame("Example/MarineMicro_6enemy.SC2Map");
	//coordinator.StartGame("Example/MarineMicro_distributed.SC2Map");
	coordinator.StartGame("Example/MarineMicro_distributed_circle.SC2Map");


	while (coordinator.Update()) {
		if (sc2::PollKeyPress()) {
			break;
		}

		if (bot.get_Game_Ended_()) {
			//break;
		}
	}

	return 0;
}
