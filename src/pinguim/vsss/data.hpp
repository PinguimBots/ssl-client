// These are meant to be pretty bare, only holding the data
// in a simple format for passing around and converting.
// They should have *no* operations.
#pragma once

#include "pinguim/aliases.hpp"

namespace pinguim::vsss
{
    template<typename NumT>
    struct point
    {
        NumT x;
        NumT y;
    };

    using rpoint = point<double>;
    using fpoint = point<float>;
    using ipoint = point<i32>;
    using upoint = point<u32>;

    template<typename NumT>
    struct rect
    {
        point<NumT> topl;
        point<NumT> bottomr;
    };

    using rrect = rect<double>;
    using frect = rect<float>;
    using irect = rect<i32>;
    using urect = rect<u32>;

    struct field
    {
        rrect bounds;
        rrect right_goal;
        rrect left_goal;
    };
}
