#pragma once

#include <complex>
#include <utility>
#include <array>

namespace pbts
{
    using point = std::complex<double>;
    // Ponto usado pelo wavefront planner (cordenada discreta).
    using wpoint = std::complex<int>;

    // auto [x, y] = pbts::to_pair( ponto );
    auto to_pair(const pbts::point& p) -> std::pair<double, double>;
    auto to_pair(const pbts::wpoint& p) -> std::pair<int, int>;
    using rect = std::array<point, 4>;

    struct field_geometry
    {
        rect left_goal_bounds;
        rect right_goal_bounds;
        rect field_bounds;
    };

    struct robot
    {
        point position;
        double orientation; // In rad.

        point velocity;
        double rotation_velocity; // In rad/sec

        int id;
    };

    struct ball
    {
        point position;
        point velocity;
    };
}
