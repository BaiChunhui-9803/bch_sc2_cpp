#include "bot_allocation.h"

bool sc2::BotAllocation::LaunchMultiGame(int argc, char* argv[], size_t pop_size, size_t step_size, State load_state) {

	// 打印命令行信息
	for (int i = 0; i < argc; i++) {
		std::cout << "command para_" << "i:" << std::endl;
		std::cout << argv[i] << std::endl;
	}

	// 游戏必要信息设置
	std::string map = "Example/MarineMicro.SC2Map";
	// bots对应Client，simulators对应coordinator
	m_bots.resize(pop_size);
	m_simulators.resize(pop_size);
	m_gameinfs.resize(pop_size);

	for (int i = 0; i < pop_size; ++i) {
		m_simulators[i].SetParticipants({
			CreateParticipant(sc2::Race::Terran, &(m_bots[i])),
			CreateComputer(sc2::Race::Zerg),
			});
		m_simulators[i].LoadSettings(argc, argv);

		// SetRealtime(false)情况下允许支持暂停
		m_simulators[i].SetRealtime(false);
		m_simulators[i].SetMultithreaded(true);
		// simulators[i].SetFeatureLayers(sc2::FeatureLayerSettings(24.0f, 64, 64, 64, 64));
		m_simulators[i].SetPortStart(port_start + port_step * i);
		m_simulators[i].SetWindowLocation(1920 + 20 * i, 20 + 20 * i);
		// 在Linux环境下需要指定文件路径
		//simulators[i].SetProcessPath("/home/baichunhui/StarCraftII/Versions/Base75689/SC2_x64");
		m_simulators[i].LaunchStarcraft();
	}

	std::vector<std::thread> start_game_threads(pop_size);
	for (size_t i = 0; i < start_game_threads.size(); ++i) {
		start_game_threads[i] = std::thread([&, i]()->void {
			m_simulators[i].StartGame(map);
		});
	}
	for (auto& t : start_game_threads) {
		t.join();
	}

#define DO_PAUSE
#ifdef DO_PAUSE
	std::vector<std::thread> run_threads(pop_size);
	for (size_t i = 0; i < run_threads.size(); ++i) {
		run_threads[i] = std::thread([&, i]()->void {
			size_t step = 1;
			// 对每一个线程执行step_size步游戏循环，但不能使其退出
			// 执行step_size步之后，应当触发暂停（可通过进入循环实现），并返回想要的数据
			while (m_simulators[i].Update()) {
				if (m_bots[i].game_load_flag_) {
					m_bots[i].save_state.LoadState(m_bots[i].save_state, m_bots[i], m_simulators[i]);
				}

				//std::cout << m_bots[i].Observation()->GetGameLoop() << std::endl;
				//if (bots[i].Observation()->GetGameLoop() != 0) {
				//	++step;
				//}
				++step;
				//std::cout << step << std::endl;
				if (step >= 40) {
					m_bots[i].game_stop_observe_flag_ = true;
				}
				if (step >= step_size) {
					m_bots[i].game_pause_flag_ = true;
					m_bots[i].game_save_flag_ = true;
					m_gameinfs[i].g_inf_score = 0;
				}
				if (step >= 2 * step_size) {
					m_bots[i].game_load_flag_ = true;
					step = 1;
				}
			}
		});
	}
	for (auto& t : run_threads) {
		t.join();
	}
#else
	std::vector<std::thread> run_threads(pop_size);
	for (size_t i = 0; i < run_threads.size(); ++i) {
		run_threads[i] = std::thread([&, i]()->void {
			simulators[i].RunUpdate(step_size);
		});
	}
	for (auto& t : run_threads) {
		t.join();
	}
#endif

	return true;
}
