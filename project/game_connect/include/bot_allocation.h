#ifndef BOT_ALLOCATION_H
#define BOT_ALLOCATION_H

#include <thread>
#include <iostream>
#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2renderer/sc2_renderer.h"
#include "sc2utils/sc2_arg_parser.h"
#include "sc2api/sc2_coordinator.h"
#include "sc2utils/sc2_manage_process.h"

#include "GA.h"
#include "Command.h"
#include "bot_allocation.h"
#include "sc2utility.h"
#include "Point2DPolar.h"
#include "mybot.h"
#include "State.h"

namespace sc2 {

class BotAllocation {

public:

private:
    int port_start = 3000;
    int port_step = 2;

    // 加载必要信息，创建多个bot和客户端
    // bot对应Client，simulators对应coordinator
    std::vector<sc2::MyConnectBot> m_bots;
    std::vector<sc2::Coordinator> m_simulators;
    std::vector<sc2::GameInfCallFor> m_gameinfs;
    std::vector<std::pair<size_t, MyScore>> m_scorer;

    State m_state_save;
public:
    std::vector<std::pair<size_t, MyScore>> getScorer() { return m_scorer; }
public:
    bool LaunchMultiGame(size_t pop_size = 1, size_t step_size = 100, std::vector<Solution> load_solutions = std::vector<Solution>(), State load_state = State());

    void pushSingleScore(size_t id, MyScore single_score);

    bool isAllSimulatorEnd();

};

}

#endif