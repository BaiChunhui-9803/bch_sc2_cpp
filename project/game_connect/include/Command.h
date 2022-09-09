#ifndef COMMAND_H
#define COMMAND_H
// an action series for a unit to take, provided to algorithms to optimize

#include <sc2api/sc2_gametypes.h>
#include <sc2api/sc2_action.h>

namespace sc2 {
    struct Command {
        //Tag c_unit_tag = 0;
        RawActions c_actions = RawActions();

        Command() = default;

        bool operator==(const Command& rhs)const {
            //return c_unit_tag == rhs.c_unit_tag && c_actions == rhs.c_actions;
            return c_actions == rhs.c_actions;
        };
        bool operator!=(const Command& rhs)const {
            return !(*this == rhs);
        };
    };
}

#endif