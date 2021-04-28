#pragma once

#include "pinguim/vsss/common.hpp"

namespace pinguim::vsss::control {
    // Returns the [x, y] velocity of the robot's wheels.
    auto generate_vels(pinguim::vsss::robot robot, pinguim::vsss::point target_pos, int rotation) -> pinguim::vsss::point;
    auto rotate(pinguim::vsss::robot robot, double angle) -> pinguim::vsss::point;
}
