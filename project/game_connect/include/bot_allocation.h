#pragma once

#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2renderer/sc2_renderer.h"
#include "sc2utils/sc2_arg_parser.h"
#include "sc2api/sc2_coordinator.h"
#include "sc2utils/sc2_manage_process.h"
#include <thread>
#include <iostream>

#include "mybot.h"
#include "State.h"

namespace sc2 {

class BotAllocation {
private:
    int port_start = 3000;
    int port_step = 2;



    // 加载必要信息，创建多个bot和客户端
    // bot对应Client，simulators对应coordinator
    std::vector<sc2::MyConnectBot> m_bots;
    std::vector<sc2::Coordinator> m_simulators;
    std::vector<sc2::GameInfCallFor> m_gameinfs;

    State m_state_save;



public:
    bool LaunchMultiGame(int argc, char* argv[], size_t pop_size = 1, size_t step_size = 100, State load_state = State());

};

}

