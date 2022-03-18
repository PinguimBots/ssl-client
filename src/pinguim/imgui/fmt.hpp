// Exposes fmtlib for formatting instead of ImGui via ImGui::TextF(...)
// and a few other goodies.
#pragma once

#include <string_view>

#include <imgui.h>
#include <fmt/core.h>

#include "pinguim/standalone/forward.hpp"
#include "pinguim/utils.hpp"

namespace pinguim::ImGui
{
    // Uses fmtlib instead of ImGui to format an ImGui::Text.
    constexpr auto TextF(auto&&... args)
    {
        return ::ImGui::Text("%s", fmt::format(s::forward<decltype(args)>(args)...).c_str());
    }

    // Makes an ImGui::Text formatted as "{prefix}: {type(v)} = {v:v_fmt}",
    // where <v_fmt> controls the formatting of <v>.
    template<bool ShortInts = true, bool ShortFloats = true, typename T>
    constexpr auto ValueT(std::string_view prefix, T&& v, std::string_view v_fmt = "")
    {
        return pinguim::ImGui::TextF(
            fmt::format("{{}}: {{}} = {{{}}}", v_fmt),
            prefix,
            pinguim::utils::type_name<T>(),
            std::forward<T>(v)
        );
    }
}
