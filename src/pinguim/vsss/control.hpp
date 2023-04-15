#pragma once

#include "pinguim/vsss/common.hpp"

namespace pinguim::vsss::control {
    // Returns the [x, y] velocity of the robot's wheels.
    auto generate_vels(pinguim::vsss::common::robot robot, pinguim::vsss::common::point target_pos, int rotation) -> pinguim::vsss::common::point;
    auto rotate(pinguim::vsss::common::robot robot, double angle) -> pinguim::vsss::common::point;
}
