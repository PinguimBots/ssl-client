#include "pinguim/imgui/widgets/toggle_button.hpp"

auto pinguim::ImGui::ToggleButton(char const* label, bool* is_toggled, ImVec2 const& size) -> bool
{
    namespace ImGui = ::ImGui;

    auto clicked = false;
    auto dummy = false;
    auto& _is_toggled = is_toggled != nullptr ? *is_toggled : dummy;

    if(_is_toggled)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
        if( ImGui::Button(label, size) ) { _is_toggled = false; clicked = true; }
        ImGui::PopStyleColor(1);
    }
    else if( ImGui::Button(label, size) ) { _is_toggled = clicked = true; }

    return clicked;
}

auto pinguim::ImGui::ToggleButton(char const* label, bool is_toggled, ImVec2 const& size) -> bool
{ return ToggleButton(label, &is_toggled, size); }
