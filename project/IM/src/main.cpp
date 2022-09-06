#include "sc2api/sc2_api.h"
#include "IMBot.h"

#include "sc2utils/sc2_arg_parser.h"
#include "sc2utils/sc2_manage_process.h"

//*************************************************************************************************
int main(int argc, char* argv[]) {
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}

	coordinator.SetWindowLocation(800, 100);
	// true: ʵ���ٶ�
	// false: ���٣�����ʹ����ͣ
	//coordinator.SetRealtime(true);
	coordinator.SetRealtime(false);

	// ����Զ���bot
	sc2::IMBot bot;

	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Terran, &bot),
		CreateComputer(sc2::Race::Zerg),
		});


	// ������Ϸ
	coordinator.LaunchStarcraft();

	// ���ص�ͼ
	//coordinator.StartGame("Example/MarineMicro_6enemy.SC2Map");
	//coordinator.StartGame("Example/MarineMicro_distributed.SC2Map");
	//coordinator.StartGame("Example/MarineMicro_1enemy.SC2Map");
	//coordinator.StartGame("Example/MarineMicro_example3.SC2Map");

	coordinator.StartGame("Example/MarineMicro_distributed_circle.SC2Map");


	while (coordinator.Update()) {
		if (sc2::PollKeyPress()) {
			break;
		}

		if (bot.get_Game_Ended_()) {
			break;
		}
	}

	return 0;
}
