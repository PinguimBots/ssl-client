#pragma once

#include <array>
#include <complex>

namespace pbts
{
    using rect = std::array< std::complex<double>, 4>;

    struct field_geometry
    {
        rect left_goal_bounds;
        rect right_goal_bounds;
        rect field_bounds;
    }; 
}
