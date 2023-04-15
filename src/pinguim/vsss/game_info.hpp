// These are meant to be pretty bare, only holding the data
// in a simple format for passing around and converting.
// They should have little to *no* operations.
//
// Positions are in Meters and Rotations are in Radians
// unless otherwise specified.
#pragma once

#include "pinguim/list.hpp"
#include "pinguim/geometry.hpp"
#include "pinguim/aliases.hpp"

namespace pinguim::vsss::inline gi
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

        float wheelbase    = 0.075f;
        float wheel_radius = 0.030f;

        // Direction unit-vectors.
        inline auto forward() const -> geo::fpoint
        { return geo::fpoint{1, 0}.rotated(rotation); }
        inline auto backward() const -> geo::fpoint
        { return geo::fpoint{-1, 0}.rotated(rotation); }
        inline auto left() const -> geo::fpoint
        { return geo::fpoint{0, 1}.rotated(rotation); }
        inline auto right() const -> geo::fpoint
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
        list<geo::fpoint> bounds;
        list<geo::fpoint> target_goal;
        list<geo::fpoint> defending_goal;
    };

    struct game_info
    {
        list<robot> allied_team;
        u8          allied_team_id;
        list<robot> enemy_team;
        field       field_info;
        ball        ball_info;
        u16         allied_goals;
        u16         enemy_goals;
    };
}
