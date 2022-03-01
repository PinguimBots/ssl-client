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

#include <vector>

#include "pinguim/vsss/game_info.hpp"
#include "pinguim/vsss/command.hpp"

// Forward decl. defined in <SDL.h>.
union SDL_Event;

namespace pinguim::app
{
    using pinguim::vsss::game_info;
    using pinguim::vsss::command;
    using commands = std::vector<command>;

    struct input_subsystem
    {
        virtual auto update_gameinfo(game_info&, float delta_seconds) -> bool = 0;
        virtual ~input_subsystem() = default;

        // Meant to be used by pinguim::app::subsystems::manager, override if you if need
        // to hook into SDL's event loop (reading keyboard, doing graphics stuff, etc).
        // Return true if the event was handled by this subsystem, false otherwise.
        virtual auto handle_event(SDL_Event&) -> bool { return false; }
    };

    struct logic_subsystem
    {
        virtual auto run_logic(game_info const&, commands&, float delta_seconds) -> bool = 0;
        virtual ~logic_subsystem() = default;

        // Meant to be used by pinguim::app::subsystems::manager, override if you if need
        // to hook into SDL's event loop (reading keyboard, doing graphics stuff, etc).
        // Return true if the event was handled by this subsystem, false otherwise.
        virtual auto handle_event(SDL_Event&) -> bool { return false; }
    };

    struct output_subsystem
    {
        virtual auto transmit(commands const&, float delta_seconds) -> bool = 0;
        virtual ~output_subsystem() = default;

        // Meant to be used by pinguim::app::subsystems::manager, override if you if need
        // to hook into SDL's event loop (reading keyboard, doing graphics stuff, etc).
        // Return true if the event was handled by this subsystem, false otherwise.
        virtual auto handle_event(SDL_Event&) -> bool { return false; }
    };
}
