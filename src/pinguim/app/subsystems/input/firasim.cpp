#include "pinguim/app/subsystems/input/firasim.hpp"

#include "pinguim/vsss/firasim/tuplified_proto.hpp"
#include "pinguim/vsss/net/decoders/sim.hpp" // Needed for receiver_t.sync().
#include "pinguim/aliases.hpp"
#include "pinguim/utils.hpp" // For emplace_fill_capacity().
#include "pinguim/cvt.hpp"

namespace pinguim::app::subsystems::input
{
    firasim::firasim(std::string_view addr, u16 port)
        : allied_team_color{team::yellow}
        , env_receiver{addr, port}
        //, referee_receiver{"addr", "port"};
    {}

    auto firasim::update_gameinfo(game_info& gi, [[maybe_unused]] float delta_seconds) -> bool
    {
        using namespace cvt;

        auto has_new_env = env_receiver.sync( env_packet );
        //auto has_new_referee = referee_receiver.sync( referee_packet );

        if(!has_new_env) { return false; }

        gi.allied_goals = allied_team_color == team::blue
            ? toe << env_packet.goals_blue()
            : toe << env_packet.goals_yellow();
        gi.enemy_goals = allied_team_color == team::yellow
            ? toe << env_packet.goals_blue()
            : toe << env_packet.goals_yellow();

        const auto [blue_team, yellow_team, ball] = env_packet.frame();
        const auto& allied_team = allied_team_color == team::yellow
            ? yellow_team
            : blue_team;
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
            auto [id, x, y, orientation, vx, vy, vorientation] = allied_team[i];
            auto& robot = gi.allied_team[i];
            robot.location = {toe << x,  toe << y};
            robot.velocity = {toe << vx, toe << vy};
            robot.rotation = toe << orientation;
            robot.angular_velocity = toe << vorientation;
            robot.size = 0;
            robot.id = toe << id;
        }
        for(auto i = 0u; i < enemy_team.size(); ++i)
        {
            auto [id, x, y, orientation, vx, vy, vorientation] = enemy_team[i];
            auto& robot = gi.enemy_team[i];
            robot.location = {toe << x,  toe << y};
            robot.velocity = {toe << vx, toe << vy};
            robot.rotation = toe << orientation;
            robot.angular_velocity = toe << vorientation;
            robot.size = 0;
            robot.id = toe << id;
        }

        const auto [x, y, z, vx, vy, vz] = ball;
        gi.ball_info.location = {toe << x,  toe << y};
        gi.ball_info.velocity = {toe << vx, toe << vy};
        gi.ball_info.radius = 0.0215f;

        const auto [fx, fy, goaly, goalx] = env_packet.field();
        gi.field_info.bounds = {
            // Top walls.
            {toe << -fx/2, toe << fy/2}, {toe << fx/2, toe << fy/2},
            // Right side goal.
            {toe << fx/2, toe << goaly/2}, {toe << fx/2+goalx, toe << goaly/2},
            {toe << fx/2+goalx, toe << -goaly/2}, {toe << fx/2, toe << -goaly/2},
            // Bottom walls.
            {toe << fx/2, toe << -fy/2}, {toe << -fx/2, toe << -fy/2},
            // Left side goal.
            {toe << -fx/2, toe << -goaly/2}, {toe << -fx/2-goalx, toe << -goaly/2},
            {toe << -fx/2-goalx, toe << goaly/2}, {toe << -fx/2, toe << goaly/2},
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
