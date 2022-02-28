#pragma once

#include "pinguim/app/subsystems/types.hpp"

namespace pinguim::app::subsystems::input
{
    struct vision : public pinguim::app::input_subsystem
    {
        auto update_gameinfo(game_info&, float delta_seconds) -> bool override;
    };
}
