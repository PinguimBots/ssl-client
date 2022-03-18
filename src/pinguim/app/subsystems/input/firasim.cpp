#include "pinguim/app/subsystems/input/firasim.hpp"

#include "pinguim/vsss/firasim/tuplified_proto.hpp"
#include "pinguim/vsss/net/decoders/sim.hpp" // Needed for receiver_t.sync().

#include "pinguim/imgui/widgets/group_panel.hpp"

#include "pinguim/app/subsystems/registrar.hpp"

#include "pinguim/aliases.hpp"
#include "pinguim/utils.hpp" // For emplace_fill_capacity().
#include "pinguim/cvt.hpp"

#include <imgui.h>
#include <fmt/core.h>

PINGUIM_APP_REGISTER_INPUT_SUBSYSTEM(pinguim::app::subsystems::input::firasim, "Firasim");

namespace pinguim::app::subsystems::input
{
    firasim::firasim(std::string_view addr, u16 port)
        : allied_team_color{team::yellow}
        , env_receiver{addr, port}
        //, referee_receiver{"addr", "port"};
    {}

    auto firasim::update_gameinfo(game_info& gi, [[maybe_unused]] float delta_seconds) -> bool
    {
        namespace ImGui = ::ImGui;

        auto has_new_env = env_receiver.sync( env_packet );
        //auto has_new_referee = referee_receiver.sync( referee_packet );

        ImGui::SetNextWindowSize({0, 0});
        ImGui::Begin("[INPUT] Firasim");
        for(auto& robot : gi.allied_team) {
            pb::ImGui::BeginGroupPanel(fmt::format("Robot {}", robot.id).c_str());
            ImGui::SetNextItemWidth(150);
            ImGui::InputFloat(
                fmt::format("Wheelbase##robot {} wheelbase", robot.id).c_str(),
                &robot.wheelbase,
                0.001f,
                0.010f,
                "%0.3f Meters"
            );
            ImGui::SetNextItemWidth(150);
            ImGui::InputFloat(
                fmt::format("Wheel Radius##robot {} wheel radius", robot.id).c_str(),
                &robot.wheel_radius,
                0.001f,
                0.010f,
                "%0.3f Meters"
            );
            ImGui::Dummy({0, 5});
            pb::ImGui::EndGroupPanel();
        }
        ImGui::End();

        if(!has_new_env) { return false; }

        gi.allied_goals = allied_team_color == team::blue
            ? env_packet.goals_blue() * cvt::toe
            : env_packet.goals_yellow() * cvt::toe;
        gi.enemy_goals = allied_team_color == team::yellow
            ? env_packet.goals_blue() * cvt::toe
            : env_packet.goals_yellow() * cvt::toe;

        const auto [blue_team, yellow_team, ball] = env_packet.frame();
        const auto& allied_team = allied_team_color == team::yellow
            ? yellow_team
            : blue_team;
        gi.allied_team_id = allied_team_color == team::yellow ? 0 : 1;
        const auto& enemy_team = allied_team_color == team::yellow
            ? blue_team
            : yellow_team;
        // Fill the teams with zeroed robots if the size differs.
        gi.allied_team.reserve(allied_team.size());
        pb::emplace_fill_capacity(gi.allied_team);
        gi.enemy_team.reserve(enemy_team.size());
        pb::emplace_fill_capacity(gi.enemy_team);
        for(auto i = 0u; i < allied_team.size(); ++i)
        {
            const auto [id, x, y, orientation, vx, vy, vorientation] = allied_team[i];
            auto& robot = gi.allied_team[i];
            robot.location = { x * cvt::toe, y * cvt::toe};
            robot.velocity = { vx * cvt::toe, vy * cvt::toe};
            robot.rotation = orientation * cvt::toe;
            robot.angular_velocity = vorientation * cvt::toe;
            robot.size = 0;
            robot.id = cvt::toe * id;
        }
        for(auto i = 0u; i < enemy_team.size(); ++i)
        {
            const auto [id, x, y, orientation, vx, vy, vorientation] = enemy_team[i];
            auto& robot = gi.enemy_team[i];
            robot.location = { x * cvt::toe,  y * cvt::toe};
            robot.velocity = { vx * cvt::toe, vy * cvt::toe};
            robot.rotation = orientation * cvt::toe;
            robot.angular_velocity = vorientation * cvt::toe;
            robot.size = 0;
            robot.id = cvt::toe * id;
        }

        const auto [x, y, z, vx, vy, vz] = ball;
        gi.ball_info.location = { x * cvt::toe,  y * cvt::toe};
        gi.ball_info.velocity = { vx * cvt::toe, vy * cvt::toe};
        gi.ball_info.radius = 0.0215f;

        const auto [fx, fy, goaly, goalx] = env_packet.field();
        gi.field_info.bounds = {
            // Top walls.
            {cvt::toe + -fx/2, cvt::toe + fy/2}, {cvt::toe + fx/2, cvt::toe + fy/2},
            // Right side goal.
            {cvt::toe + fx/2, cvt::toe + goaly/2}, {cvt::toe + (fx/2+goalx), cvt::toe + goaly/2},
            {cvt::toe + (fx/2+goalx), cvt::toe + -goaly/2}, {cvt::toe + fx/2, cvt::toe + -goaly/2},
            // Bottom walls.
            {cvt::toe + fx/2, cvt::toe + -fy/2}, {cvt::toe + -fx/2, cvt::toe + -fy/2},
            // Left side goal.
            {cvt::toe + -fx/2, cvt::toe + -goaly/2}, {cvt::toe + (-fx/2-goalx), cvt::toe + -goaly/2},
            {cvt::toe + (-fx/2-goalx), cvt::toe + goaly/2}, {cvt::toe + -fx/2, cvt::toe + goaly/2}
        };
        const auto target_goal_indexes = allied_team_color == team::blue
            ? pb::arr(2, 6)
            : pb::arr(8, 12);
        const auto defending_goal_indexes = allied_team_color == team::yellow
            ? pb::arr(2, 6)
            : pb::arr(8, 12);
        gi.field_info.target_goal = {
            gi.field_info.bounds.begin() + target_goal_indexes[0],
            gi.field_info.bounds.begin() + target_goal_indexes[1]
        };
        gi.field_info.defending_goal = {
            gi.field_info.bounds.begin() + defending_goal_indexes[0],
            gi.field_info.bounds.begin() + defending_goal_indexes[1]
        };

        return true;
    }
}
