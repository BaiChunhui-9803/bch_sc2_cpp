#include "sc2api/sc2_api.h"
#include "bot_allocation.h"
#include "mybot.h"

#include "sc2utils/sc2_arg_parser.h"
#include "sc2api/sc2_coordinator.h"
#include "sc2utils/sc2_manage_process.h"

#include <thread>

#define BUILD_FOR_MULTI_THREADS
#ifdef BUILD_FOR_MULTI_THREADS
//*************************************************************************************************
int main(int argc, char* argv[]) {
using namespace std;

	// 设置客户端数量、游戏步长
	int POPSIZE = 1;
	int STEPSIZE = 50;

	sc2::BotAllocation bot_manager;
	bot_manager.LaunchMultiGame(argc, argv, POPSIZE, STEPSIZE);





	//std::vector<sc2::MyConnectBot> bots;
	//bots.resize(pop_size);

	//std::vector<sc2::Coordinator> simulators;
	//simulators.resize(pop_size);

	//for (int i = 0; i < pop_size; ++i) {
	//	simulators[i].SetParticipants({
	//		CreateParticipant(sc2::Race::Terran, &(bots[i])),
	//		CreateComputer(sc2::Race::Zerg),
	//	});
	//	simulators[i].LoadSettings(argc, argv);

	//	// SetRealtime(false)情况下允许支持暂停
	//	simulators[i].SetRealtime(true);
	//	// simulators[i].SetFeatureLayers(sc2::FeatureLayerSettings(24.0f, 64, 64, 64, 64));
	//	simulators[i].SetPortStart(3000 + 2 * i);
	//	simulators[i].SetWindowLocation(1920 + 20 * i, 20 + 20 * i);
	//	//simulators[i].SetProcessPath("/home/baichunhui/StarCraftII/Versions/Base75689/SC2_x64");
	//	simulators[i].LaunchStarcraft();
	//}

	/*std::vector<std::thread> start_game_threads(pop_size);
	for (size_t i = 0; i < start_game_threads.size(); ++i) {
		start_game_threads[i] = std::thread([&, i]()->void{
			simulators[i].StartGame("Example/MarineMicro.SC2Map");
		});
	}
	for (auto& t : start_game_threads) {
		t.join();
	}*/

////#define DO_PAUSE
//#ifdef DO_PAUSE
//	std::vector<std::thread> run_threads(pop_size);
//	for (size_t i = 0; i < run_threads.size(); ++i) {
//		run_threads[i] = std::thread([&, i]()->void{
//			while (simulators[i].Update()) {
//				if (sc2::PollKeyPress()) {
//					break;
//				}
//			}
//		});
//	}
//	for (auto& t : run_threads) {
//		t.join();
//	}
//#else
//	std::vector<std::thread> run_threads(pop_size);
//	for (size_t i = 0; i < run_threads.size(); ++i) {
//		run_threads[i] = std::thread([&, i]()->void {
//			simulators[i].RunUpdate(step_size);
//		});
//	}
//	for (auto& t : run_threads) {
//		t.join();
//	}
//#endif

	// MicroAgentBot bot;

	// 启动游戏
	// coordinator.SetPortStart(3000);
	// coordinator.SetParticipants({
	// 	CreateParticipant(sc2::Race::Terran, &bot),
	// 	CreateComputer(sc2::Race::Zerg),
	// });


	// coordinator.SetPortStart(3000);
	// coordinator.SetProcessPath("/home/baichunhui/StarCraftII/Versions/Base75689/SC2_x64");
	// coordinator.LaunchStarcraft();
	// coordinator.StartGame("Example/MarineMicro.SC2Map");



	// while (coordinator.Update()) {
	// 	if (sc2::PollKeyPress()) {
	// 		break;
	// 	}

	// 	if (bot.get_Game_Ended_()) {
	// 		sc2::renderer::Shutdown();
	// 		break;
	// 	}
	// }
}
#else
//*************************************************************************************************
int main(int argc, char* argv[]) {
using namespace std;

	for (int i = 0; i < argc; i++) {
		std::cout << "command para_" << "i:" << std::endl;
        std::cout << argv[i] << std::endl;
    }
	// 添加自定义bot
	sc2::MyConnectBot bot;

	// 启动游戏
	sc2::Coordinator coordinator;
	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Terran, &bot),
		CreateComputer(sc2::Race::Zerg),
	});
	coordinator.SetPortStart(3000);
	// coordinator.SetFeatureLayers(sc2::FeatureLayerSettings(24.0f, 64, 64, 64, 64));
	coordinator.SetProcessPath("/home/baichunhui/StarCraftII/Versions/Base75689/SC2_x64");
	coordinator.LaunchStarcraft();
	coordinator.StartGame("Example/MarineMicro.SC2Map");

	while (coordinator.Update()) {
		if (sc2::PollKeyPress()) {
			break;
		}

		if (bot.get_Game_Ended_()) {
			// sc2::renderer::Shutdown();
			break;
		}
	}
}
#endif