// These are meant to be pretty bare, only holding the data
// in a simple format for passing around and converting.
// They should have little to *no* operations.
//
// Positions are in Meters and Rotations are in Radians
// unless otherwise specified.
#pragma once

#include <vector>

#include "pinguim/aliases.hpp"
#include "pinguim/geometry.hpp"

namespace pinguim::vsss
{
    struct robot
    {
        geo::fpoint location;
        // Convetionally in Meters/Second.
        geo::fpoint velocity;
        // Conventionally anti-clockwise starting from the right.
        float rotation;
        float angular_velocity;
        float size;
        u8 id;

        // Direction unit-vectors.
        constexpr auto forward() -> geo::fpoint
        { return geo::fpoint{1, 0}.rotated(rotation); }
        constexpr auto backward() -> geo::fpoint
        { return geo::fpoint{-1, 0}.rotated(rotation); }
        constexpr auto left() -> geo::fpoint
        { return geo::fpoint{0, 1}.rotated(rotation); }
        constexpr auto right() -> geo::fpoint
        { return geo::fpoint{0, -1}.rotated(rotation); }
    };

    struct ball
    {
        geo::fpoint location;
        geo::fpoint velocity;
        float radius;
    };

    struct field
    {
        std::vector<geo::fpoint> bounds;
        std::vector<geo::fpoint> target_goal;
        std::vector<geo::fpoint> defending_goal;
    };

    struct game_info
    {
        std::vector<robot> allied_team;
        std::vector<robot> enemy_team;
        field field_info;
        ball ball_info;
        u16 allied_goals;
        u16 enemy_goals;
    };
}
