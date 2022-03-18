#pragma once

#include "pinguim/vsss/game_info.hpp"
#include "pinguim/vsss/command.hpp"
#include "pinguim/geometry.hpp"

namespace pinguim::vsss::control
{
    enum class special_action {
        none,
        rotate_anti_clockwise = 1,
        rotate_clockwise = 2,
    };

    // Returns the [x, y] velocity of the robot's wheels.
    auto goto_point(pinguim::vsss::robot robot, pinguim::geo::fpoint target, special_action action) -> command;
}
