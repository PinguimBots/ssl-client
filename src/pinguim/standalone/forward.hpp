#pragma once

#include "pinguim/standalone/remove_cvref.hpp"

namespace pinguim::s
{
    template <typename T>
    constexpr auto forward(remove_ref_t<T>& t) noexcept -> T&&
    { return static_cast<T&&>(t); }

    template <typename T>
    constexpr auto forward(remove_ref_t<T>&& t) noexcept -> T&&
    { return static_cast<T&&>(t); }

    template<typename T>
    constexpr auto move(T&& t) noexcept -> remove_ref_t<T>&&
    { return static_cast<remove_ref_t<T>&&>(t); }
}
