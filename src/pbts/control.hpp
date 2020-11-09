#pragma once

#include "pbts/common.hpp"

namespace pbts::control {
    // Returns the [x, y] velocity of the robot's wheels.
    auto generate_vels(pbts::robot robot, pbts::point target_pos, int rotation) -> pbts::point;
    auto rotate(pbts::robot robot, double angle) -> pbts::point;
}
