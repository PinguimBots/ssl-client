#pragma once

#include "pinguim/app/subsystem_types.hpp"

namespace pinguim::app::subsystems::logic
{
    struct strategy : public pinguim::app::logic_subsystem
    {
        auto run_logic(game_info const&, commands&, float delta_seconds) -> bool override;
    };
}