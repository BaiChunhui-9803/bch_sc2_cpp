#include "bot_allocation.h"

bool sc2::BotAllocation::LaunchMultiGame(size_t pop_size, size_t step_size, std::vector<Command> load_commands, State load_state) {

	// 游戏必要信息设置
	std::string map = "Example/MarineMicro.SC2Map";
	std::string blank_map = "Example/BlankMap.SC2Map";
	// bots对应Client，simulators对应coordinator
	m_bots.resize(pop_size);
	m_simulators.resize(pop_size);
	m_gameinfs.resize(pop_size);

	for (int i = 0; i < pop_size; ++i) {
		m_simulators[i].SetParticipants({
			CreateParticipant(sc2::Race::Terran, &(m_bots[i])),
			CreateComputer(sc2::Race::Zerg,
			sc2::Difficulty::VeryEasy,
			sc2::AIBuild::Macro,
			"VeryEasy"),
			});
		//m_simulators[i].LoadSettings(argc, argv);
		m_simulators[i].SetProcessPath("C:\\Program Files (x86)\\StarCraft II\\Versions\\Base87702\\SC2.exe");

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
			if (load_state.isBlank()) {
				m_simulators[i].StartGame(map);
			}
			else {
				m_simulators[i].StartGame(blank_map);
			}
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
				if (isAllSimulatorEnd()) {
					m_scorer.push_back(std::pair<size_t, MyScore>(i, m_bots[i].m_scorer));
					//m_simulators[0].LeaveGame();
					break;
				}
				if (m_bots[i].game_idle_flag) {
					m_bots[i].game_leave_flag = true;
				}
				else {
					if (!m_bots[i].game_load_finish_flag_ && !m_bots[i].game_load_flag_ && !load_state.isBlank()) {
						m_bots[i].game_load_flag_ = true;
					}

					// 暂时注释
					//if (m_bots[i].game_load_flag_) {
					if (!m_bots[i].flag_test) {
						m_bots[i].save_state.LoadState(load_state, m_bots[i], m_simulators[i], load_commands);
						m_bots[i].observed_units.clear();
						m_bots[i].game_load_flag_ = false;
						m_bots[i].game_load_finish_flag_ = true;
						m_bots[i].flag_test = true;
					}

					//std::cout << m_bots[i].Observation()->GetGameLoop() << std::endl;
					//if (bots[i].Observation()->GetGameLoop() != 0) {
					//	++step;
					//}
					++step;
					//std::cout << step << std::endl;
					if (step >= step_size - 10) {
						m_bots[i].game_stop_observe_flag_ = true;
					}
					if (step >= step_size) {
						m_bots[i].game_pause_flag_ = true;
						m_bots[i].game_save_flag_ = true;
						//m_gameinfs[i].g_inf_score = 0;
						//pushSingleScore(i, m_bots[i].getScore());
						//std::vector<std::pair<size_t, MyScore>> scorer = getScorer();
						//for()
						//	std::cout << "scorer[" << m_scorer.at(i).first << "]\t"
						//		<< "damage_to_enemy:" << m_scorer.at(i).second.m_damage_to_enemy << "\t"
						//		<< "damage_to_self:" << m_scorer.at(i).second.m_damage_to_self << "\t"
						//		<< "total_score:" << m_scorer.at(i).second.m_total_score << std::endl;
						//std::cout << m_bots[i].getScore();
					}
					if (m_bots[i].game_pause_finish_flag_) {
						std::cout << "分数:" << m_bots[i].m_scorer.m_total_score << std::endl;
						m_bots[i].game_idle_flag = true;
					}
					//if (step >= 2 * step_size) {
					//	m_bots[i].game_load_flag_ = true;
					//	step = 1;
					//}
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

void sc2::BotAllocation::pushSingleScore(size_t id, MyScore single_score) {
	std::cout << "id:" << id << "\tm_total_score:" << single_score.m_total_score << std::endl;
	m_scorer.push_back(std::pair<size_t, MyScore>(id, single_score));
}

bool sc2::BotAllocation::isAllSimulatorEnd() {
	for (size_t i = 0; i < m_bots.size(); ++i) {
		if (!m_bots[i].game_leave_flag) {
			return false;
		}
	}
	return true;
}

