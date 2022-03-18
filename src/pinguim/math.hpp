#pragma once

#include <algorithm>
#include <cmath>

namespace pinguim::math
{
    template <typename ReturnT = int>
    constexpr auto sign(auto v) { return v >= decltype(v){0} ? ReturnT{1} : ReturnT{-1}; }

    // Saturating lerp.
    constexpr auto satlerp(auto now, auto target, auto factor)
    {
        using ft = decltype(factor);
        factor = std::clamp(factor, ft{0}, ft{1});
        return std::lerp(now, target, factor);
    }
}

namespace pinguim{ namespace m = pinguim::math; }
