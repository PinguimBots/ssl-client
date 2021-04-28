#pragma once

#include "pinguim/common.hpp"

namespace pinguim::control {
    // Returns the [x, y] velocity of the robot's wheels.
    auto generate_vels(pinguim::robot robot, pinguim::point target_pos, int rotation) -> pinguim::point;
    auto rotate(pinguim::robot robot, double angle) -> pinguim::point;
}
