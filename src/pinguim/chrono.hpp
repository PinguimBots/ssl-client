#pragma once

#include <chrono>

namespace pinguim::inline chrono
{
    template<
        typename Timer    = std::chrono::high_resolution_clock,
        typename Duration = std::chrono::duration<float>,
        typename Functor
    >
    inline auto time(Functor&& f)
    {
        auto t1 = Timer::now();
        f();
        auto t2 = Timer::now();
        return std::chrono::duration_cast<Duration>(t2 - t1).count();
    }
}