#pragma once

#include "pinguim/standalone/forward.hpp"

namespace pinguim
{
    template <typename Func>
    struct dont_forget
    {
        constexpr dont_forget(Func&& oe) : on_exit{ s::forward<Func>(oe) } {}

        dont_forget() = delete;
        dont_forget(dont_forget&&) = delete;
        dont_forget(const dont_forget&) = delete;
        auto operator=(dont_forget&&) -> dont_forget& = delete;
        auto operator=(const dont_forget&) -> dont_forget& = delete;

        ~dont_forget() { on_exit(); }

    private:
        Func on_exit;
    };
}

#define _PINGUIM_DONT_FORGET_CONCAT_IMPL(x, y) x##y
#define _PINGUIM_DONT_FORGET_CONCAT(x, y) _PINGUIM_DONT_FORGET_CONCAT_IMPL(x, y)

#define PINGUIM_DONT_FORGET( body ) \
    auto _PINGUIM_DONT_FORGET_CONCAT(pinguim_reminder_on_line_, __LINE__) = pinguim::dont_forget{ [&]{ body; }}
