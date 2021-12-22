#pragma once

#include <chrono>

namespace pinguim {
    template<
        typename Interval = std::chrono::milliseconds,
        typename Timer    = std::chrono::high_resolution_clock,
        typename Functor
    >
    inline auto time(Functor&& f)
    {
        auto t = Timer::now();
        f();
        auto t2 = Timer::now();
        return std::chrono::duration_cast<Interval>(t2 - t);
    }
}