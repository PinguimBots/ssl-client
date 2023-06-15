#pragma once

#include "pinguim/app/subsystems/input/vision/colors.hpp"

#include "pinguim/imgui/widgets/toggle_button.hpp"
#include "pinguim/imgui/widgets/group_panel.hpp"

#include "pinguim/dont_forget.hpp"

#include <imgui.h>
#include <fmt/core.h>

namespace pinguim::ImGui
{
    // Returns the color to be picked by the color picker.
    inline auto ColorRangeEdit(
        const char* label,
        pinguim::app::subsystems::input::vision_impl::ColorRange& c,
        pinguim::app::subsystems::input::vision_impl::Color* colorpicker_target = nullptr
    ) -> pinguim::app::subsystems::input::vision_impl::Color*
    {
        namespace ImGui = ::ImGui;

        pb::ImGui::BeginGroupPanel(label);
        PINGUIM_DONT_FORGET( pb::ImGui::EndGroupPanel() );

        pb::ImGui::ToggleButton(fmt::format("Centered##{}", label).c_str(), &c.is_centered);
        if(c.is_centered)
        {
            ImGui::SameLine();
            pb::ImGui::ToggleButton(fmt::format("Use full range##{}", label).c_str(), &c.using_full_range);
        }
        if(c.is_centered && !c.using_full_range)
        {

        }

        auto const flags = ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV;
        auto min = c.first();
        auto max = c.second();
        ImGui::SameLine();
        ImGui::ColorButton("##minColor", ImVec4{min.r, min.g, min.b, 1}, flags);
        ImGui::SameLine();
        ImGui::ColorButton("##maxColor", ImVec4{max.r, max.g, max.b, 1}, flags);

        if(c.is_centered) { ImGui::ColorEdit3( fmt::format("Center##{}", label).c_str(), c.center.raw, flags ); }
        else              { ImGui::ColorEdit3( fmt::format("Min   ##{}", label).c_str(), c.color1.raw, flags ); }
        ImGui::SameLine();
        if(pb::ImGui::ToggleButton(fmt::format("ColorPicker##{}min", label).c_str(), colorpicker_target == &(c.color1)))
        { colorpicker_target = &(c.color1); }

        if(c.is_centered) { ImGui::ColorEdit3( fmt::format("Range ##{}", label).c_str(), c.range.raw,  flags ); }
        else              { ImGui::ColorEdit3( fmt::format("Max   ##{}", label).c_str(), c.color2.raw, flags ); }
        ImGui::SameLine();
        if(pb::ImGui::ToggleButton(fmt::format("ColorPicker##{}max", label).c_str(), colorpicker_target == &(c.color2)))
        { colorpicker_target = &(c.color2); }

        return colorpicker_target;
    }
}