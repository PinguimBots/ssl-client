#pragma once

#include "pinguim/app/subsystems/types.hpp"

namespace pinguim::app::subsystems::logic
{
    struct direct_control : public pinguim::app::logic_subsystem
    {
        auto run_logic(game_info const&, commands&, float delta_seconds) -> bool override;
    };
}