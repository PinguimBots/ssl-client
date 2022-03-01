#include "pinguim/app/subsystems/logic/direct_control.hpp"

#include <SDL.h>

#include <imgui.h>

#include <algorithm> // For std::clamp.
#include <cmath>     // For std::lerp.

#include "pinguim/imgui/widgets/group_panel.hpp"
#include "pinguim/enums.hpp"
#include "pinguim/utils.hpp" // For emplace_fill_capacity.
#include "pinguim/cvt.hpp"

namespace pinguim::app::subsystems::logic
{
    auto direct_control::run_logic(game_info const& gi, commands& c, float delta_seconds) -> bool
    {
        c.reserve(gi.allied_team.size());
        emplace_fill_capacity(c);
        robot_input_types.reserve(gi.allied_team.size());
        emplace_fill_capacity(robot_input_types, input_types::none);
        prev_inputs.reserve(gi.allied_team.size());
        emplace_fill_capacity(prev_inputs, 0, 0, 0);

        if(draw_window) { draw_inputs_window(gi, c, delta_seconds); }

        ImGui::BeginMainMenuBar();
        ImGui::EndMainMenuBar();

        return true;
    }
}

#include <fmt/core.h>

auto pinguim::app::subsystems::logic::direct_control::draw_inputs_window(game_info const& gi, commands& c, float delta_seconds) -> void
{
    const auto max_lerp_time = 0.1f;
    const auto lerp_factor   = std::clamp(delta_seconds, 0.f, max_lerp_time) / max_lerp_time;

    ImGui::SetNextWindowSize({610, 237}, ImGuiCond_Once);
    if(ImGui::Begin("[Logic] Direct Control", &draw_window))
    {
        for(auto i = 0u; i < c.size(); ++i)
        {
            auto const robot_id = gi.allied_team[i].id;

            ImGui::BeginGroupPanel(fmt::format("Robot {} Input Panel", robot_id).c_str());
            ImGui::Dummy({0, 5});

            auto& selected = robot_input_types[i];
            draw_input_type_combo(robot_id, selected);

            auto curr_input = command{};
            auto keys = SDL_GetKeyboardState(nullptr);
            if(selected == input_types::keyboard_AD)
            { curr_input = {cvt::toe << robot_id, cvt::toe << keys[SDL_SCANCODE_A]    * 255, cvt::toe << keys[SDL_SCANCODE_D]     * 255}; }
            else if(selected == input_types::keyboard_JL)
            { curr_input = {cvt::toe << robot_id, cvt::toe << keys[SDL_SCANCODE_J]    * 255, cvt::toe << keys[SDL_SCANCODE_L]     * 255}; }
            else if(selected == input_types::keyboard_LEFTRIGHT)
            { curr_input = {cvt::toe << robot_id, cvt::toe << keys[SDL_SCANCODE_LEFT] * 255, cvt::toe << keys[SDL_SCANCODE_RIGHT] * 255}; }
            else { curr_input = {cvt::toe << robot_id, 0, 0}; }

            const auto min = 0_u8;
            const auto max = 255_u8;

            // TODO: The 2 sliders are in ever so slightly different heights for some reason.
            ImGui::BeginGroup();
            ImGui::Text("Left Motor");
            ImGui::VSliderScalar(
                fmt::format("##robot {} left motor", robot_id).c_str(),
                {80, 100},
                ImGuiDataType_U8,
                &curr_input.left_motor,
                &min,
                &max,
                nullptr,
                ImGuiSliderFlags_NoInput
            );
            ImGui::EndGroup();

            ImGui::SameLine(100);

            ImGui::BeginGroup();
            ImGui::Text("Right Motor");
            ImGui::VSliderScalar(
                fmt::format("##robot {} right motor", robot_id).c_str(),
                {80, 100},
                ImGuiDataType_U8,
                &curr_input.right_motor,
                &min,
                &max,
                nullptr,
                ImGuiSliderFlags_NoInput
            );
            ImGui::EndGroup();

            ImGui::EndGroupPanel();

            c[i].left_motor = cvt::toe << std::lerp(
                cvt::to<float> << prev_inputs[i].left_motor,
                cvt::to<float> << curr_input.left_motor,
                lerp_factor
            );
            c[i].right_motor = cvt::toe << std::lerp(
                cvt::to<float> << prev_inputs[i].right_motor,
                cvt::to<float> << curr_input.right_motor,
                lerp_factor
            );
            c[i].robot_id = robot_id;
            prev_inputs[i] = curr_input;

            if(i + 1 != c.size()) { ImGui::SameLine(); }
        }
    }
    ImGui::End();
}

auto pinguim::app::subsystems::logic::direct_control::draw_input_type_combo(u32 robot_id, input_types& selected) -> void
{
    auto selected_str = std::string{ pinguim::enums::unqualified_value_name(selected) };

    ImGui::BeginGroup();
    ImGui::Indent(10);
    ImGui::Text("Input device:");
    ImGui::SetNextItemWidth(160);
    if(ImGui::BeginCombo(fmt::format("##input type for robot {}", robot_id).c_str(), selected_str.c_str()))
    {
        for(const auto& [enum_value, view] : pinguim::enums::reflect<input_types>::enum_iterator())
        {
            if( ImGui::Selectable(std::string{ view.unqualified() }.c_str(), enum_value == selected) ) { selected = enum_value; }
        }
        ImGui::EndCombo();
    }
    ImGui::Dummy({0, 10});
    ImGui::EndGroup();
}
