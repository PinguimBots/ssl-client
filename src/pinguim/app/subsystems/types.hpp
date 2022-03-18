#pragma once

// There are 3 subsystem types:
//
// Input subsystems provide pinguim::vsss::game_info.
//
// Logic subsystems make decisions based on the pinguim::vsss::game_info and provide pinguim::vsss::command.
//
// Output subsystems transmit the pinguim::vsss::command to some endpoint which executes them,
// the assumption being that those commands will be performed by robots and in turn affect the
// Information subsystem's intel.

#include "pinguim/vsss/game_info.hpp"
#include "pinguim/vsss/command.hpp"
#include "pinguim/container.hpp"

namespace pinguim::app
{
    using pinguim::vsss::game_info;
    using pinguim::vsss::command;
    using commands = container<command>;

    struct input_subsystem
    {
        virtual auto update_gameinfo(game_info&, float delta_seconds) -> bool = 0;
        virtual ~input_subsystem() = default;
    };

    struct logic_subsystem
    {
        virtual auto run_logic(game_info const&, commands&, float delta_seconds) -> bool = 0;
        virtual ~logic_subsystem() = default;
    };

    struct output_subsystem
    {
        virtual auto transmit(commands const&, float delta_seconds) -> bool = 0;
        virtual ~output_subsystem() = default;
    };

    struct misc_subsystem
    {
        virtual auto loop(float delta_seconds) -> void = 0;
        virtual ~misc_subsystem() = default;
    };
}
