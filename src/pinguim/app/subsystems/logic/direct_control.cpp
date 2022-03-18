#include "pinguim/app/subsystems/logic/direct_control.hpp"

#include <SDL.h>

#include <imgui.h>

#include <fmt/core.h>

#include <algorithm> // For std::clamp.
#include <cmath>     // For std::lerp.

#include "pinguim/imgui/widgets/group_panel.hpp"
#include "pinguim/app/subsystems/registrar.hpp"
#include "pinguim/vsss/control2.hpp" // For goto_point.
#include "pinguim/dont_forget.hpp"
#include "pinguim/renum.hpp"
#include "pinguim/utils.hpp" // For emplace_fill_capacity.
#include "pinguim/list.hpp"
#include "pinguim/cvt.hpp"

PINGUIM_APP_REGISTER_LOGIC_SUBSYSTEM(pinguim::app::subsystems::logic::direct_control, "Direct control");

namespace pinguim::app::subsystems::logic
{
    auto direct_control::run_logic(game_info const& gi, commands& c, float delta_seconds) -> bool
    {
        auto lc = list<command>::from_container( s::move(c) );
        PINGUIM_DONT_FORGET(c = lc.surrender());

        lc.reserve(gi.allied_team.size());
        emplace_fill_capacity(lc);
        robot_input_types.reserve(gi.allied_team.size());
        emplace_fill_capacity(robot_input_types, input_types::none);
        prev_inputs.reserve(gi.allied_team.size());
        emplace_fill_capacity(prev_inputs);
        max_lerp_time.reserve(gi.allied_team.size());
        emplace_fill_capacity(max_lerp_time, 0.2f);

        draw_inputs_window(gi, lc, delta_seconds);

        return true;
    }
}

auto pinguim::app::subsystems::logic::direct_control::draw_inputs_window(game_info const& gi, list<command>& c, float delta_seconds) -> void
{
    namespace ImGui = ::ImGui;

    ImGui::SetNextWindowSize({0, 0});

    ImGui::Begin("[Logic] Direct Control");
    PINGUIM_DONT_FORGET( ImGui::End() );

    for(auto i = 0u; i < c.size(); ++i)
    {
        auto const robot_id = gi.allied_team[i].id;

        pb::ImGui::BeginGroupPanel(fmt::format("Robot {} Input Panel", robot_id).c_str());
        ImGui::Dummy({5, 5});

        auto& selected = robot_input_types[i];
        draw_input_type_combo(robot_id, selected, max_lerp_time[i]);

        auto target_input = command{gi.allied_team_id, robot_id * cvt::toe, 0, 0};
        auto const [fwd, bkw, left, right] = get_input_state(selected);
        // TODO: refactor into function.
        if(!is_direct(selected))
        {
            auto const final_direction =
                gi.allied_team[i].location +
                (fwd  * cvt::to<float> * gi.allied_team[i].forward() +
                bkw   * cvt::to<float> * gi.allied_team[i].backward() +
                left  * cvt::to<float> * gi.allied_team[i].left() +
                right * cvt::to<float> * gi.allied_team[i].right())
                .normalized_or({0, 0});

            if(final_direction.distance(gi.allied_team[i].location) >= 0.5f)
            {
                auto const command = pinguim::vsss::control::goto_point(
                    gi.allied_team[i],
                    final_direction,
                    vsss::control::special_action::none
                );
                target_input.left_motor  = command.left_motor;
                target_input.right_motor = command.right_motor;
            }
        }
        else
        {
            if(fwd)
            {
                target_input.left_motor  = left;
                target_input.right_motor = right;
            }
            else if(bkw)
            {
                target_input.left_motor  = -cvt::to<float>(left);
                target_input.right_motor = -cvt::to<float>(right);
            }
        }

        const auto lerp_factor   = std::clamp(delta_seconds, 0.f, max_lerp_time[i]) / max_lerp_time[i];
        auto lerped_input = command{
            target_input.team_id, target_input.robot_id,
            std::lerp(prev_inputs[i].left_motor,  target_input.left_motor,  lerp_factor),
            std::lerp(prev_inputs[i].right_motor, target_input.right_motor, lerp_factor)
        };

        // TODO: The 2 sliders are in ever so slightly different heights for some reason.
        ImGui::BeginGroup();
        ImGui::Text("Left Motor");
        ImGui::VSliderFloat(
            fmt::format("##robot {} left motor", robot_id).c_str(),
            {80, 100},
            &lerped_input.left_motor,
            -1.f, 1.f,
            "%.3f",
            ImGuiSliderFlags_NoInput
        );
        ImGui::EndGroup();

        ImGui::SameLine(100);

        ImGui::BeginGroup();
        ImGui::Text("Right Motor");
        ImGui::VSliderFloat(
            fmt::format("##robot {} right motor", robot_id).c_str(),
            {80, 100},
            &lerped_input.right_motor,
            -1.f, 1.f,
            "%.3f",
            ImGuiSliderFlags_NoInput
        );
        ImGui::EndGroup();

        pb::ImGui::EndGroupPanel();

        prev_inputs[i] = lerped_input;
        c[i] = lerped_input;

        if(i + 1 != c.size()) { ImGui::SameLine(); }
    }

}

auto pinguim::app::subsystems::logic::direct_control::draw_input_type_combo(u32 robot_id, input_types& selected, float& max_robot_lerp_time) -> void
{
    namespace ImGui = ::ImGui;

    auto selected_str = std::string{ pinguim::renum::unqualified_value_name(selected) };

    ImGui::BeginGroup();
    ImGui::Indent(10);
    ImGui::Text("Input device:");
    ImGui::SetNextItemWidth(160);
    if(ImGui::BeginCombo(fmt::format("##input type for robot {}", robot_id).c_str(), selected_str.c_str()))
    {
        for(const auto& [enum_value, view] : pinguim::renum::reflect<input_types>::enum_iterator())
        {
            if( ImGui::Selectable(std::string{ view.unqualified() }.c_str(), enum_value == selected) ) { selected = enum_value; }
        }
        ImGui::EndCombo();
    }
    ImGui::Text("Max Lerp Time");
    ImGui::SetNextItemWidth(160);
    ImGui::SliderFloat(
        fmt::format("##robot {} max lerp time", robot_id).c_str(),
        &max_robot_lerp_time,
        0.f,
        1.f,
        "%.2f Seconds"
    );
    ImGui::Dummy({0, 10});
    ImGui::EndGroup();
}

auto pinguim::app::subsystems::logic::direct_control::get_input_state(input_types input) -> input_state
{
    auto keys = SDL_GetKeyboardState(nullptr);
    auto k = [&](auto scancode){ return static_cast<bool>(keys[scancode]); };
    switch(input)
    {
        case input_types::none: return {};
        case input_types::keyboard_WASD: [[fallthrough]];
        case input_types::keyboard_WASD_direct:
            return { k(SDL_SCANCODE_W), k(SDL_SCANCODE_S), k(SDL_SCANCODE_A), k(SDL_SCANCODE_D) };
        case input_types::keyboard_IJKL: [[fallthrough]];
        case input_types::keyboard_IJKL_direct:
            return { k(SDL_SCANCODE_I), k(SDL_SCANCODE_K), k(SDL_SCANCODE_J), k(SDL_SCANCODE_L) };
        case input_types::keyboard_ARROWS: [[fallthrough]];
        case input_types::keyboard_ARROWS_direct:
            return { k(SDL_SCANCODE_UP), k(SDL_SCANCODE_DOWN), k(SDL_SCANCODE_LEFT), k(SDL_SCANCODE_RIGHT) };
    }

    return {}; // Shut up, compiler!
}
