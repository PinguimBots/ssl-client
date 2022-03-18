#pragma once

#include <imgui.h>

namespace pinguim::ImGui
{
    // A button that behaves like a checkbox.
    auto ToggleButton(char const* label, bool* is_toggled, ImVec2 const& size = {0, 0}) -> bool;
    // Just a convenience overload.
    auto ToggleButton(char const* label, bool  is_toggled, ImVec2 const& size = {0, 0}) -> bool;
}
