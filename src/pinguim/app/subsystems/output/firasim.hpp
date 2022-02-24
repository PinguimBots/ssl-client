#pragma once

#include "pinguim/app/subsystems/types.hpp"

namespace pinguim::app::subsystems::output
{
    struct firasim : public pinguim::app::output_subsystem
    {
        auto transmit(commands const&, float delta_seconds) -> bool override;
    };
}