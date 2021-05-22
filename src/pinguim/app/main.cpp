#include <QCoreApplication>

#include <fmt/core.h>

#include <complex>
#include <csignal>
#include <cstdlib> // For std::exit;

#include "pinguim/vsss/simulator_connection.hpp"
#include "pinguim/vsss/tuplified_proto.hpp"
#include "pinguim/vsss/strategy.hpp"
#include "pinguim/vsss/control.hpp"
#include "pinguim/app/cmdline.hpp"
#include "pinguim/cvt.hpp"

using pinguim::cvt::toe;
using pinguim::cvt::tou;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // We dont want Qt to hijack the command line arguments
    // so we'll just lie saying that there were none.
    // Also, why doesn't it take an int but an int&, nonsense.
    auto fake_arg_count = 1;
    auto app = QCoreApplication{fake_arg_count, argv};
    std::signal(SIGINT, []([[maybe_unused]] int signal) {
        fmt::print("SIGINT Received, Closing\n");
        QCoreApplication::exit(0);
    });
    std::signal(SIGABRT, []([[maybe_unused]] int signal) {
        fmt::print("SIGABRT Received. Something went bad!\n");
        QCoreApplication::exit(1);
        std::exit(1);
    });

    auto args = pinguim::app::cmdline::parse_argv_or_abort(
        argc,
        const_cast<const char**>(argv)
    );

    const auto is_yellow = args.team == "yellow";

    auto bounds = std::optional<pinguim::vsss::field_geometry>{};

    auto strategy = pinguim::vsss::Strategy();

    strategy.setTeam(is_yellow);

    bool game_on = true;

    pinguim::vsss::simulator_connection VSSS{
        {args.in_address,  args.in_port},
        {args.out_address, args.out_port},
        /* on_simulator_receive= */ [&](auto environment) {
            /// TODO: measure timing

            /// TODO: Make sure it's necessary to verify has_frame() and has_field().
            if (!environment.has_field())
            {
                fmt::print("MESSAGE_ERROR: NO FIELD (has_field() == false)\n");
                return;
            }
            else if (!environment.has_frame())
            {
                fmt::print("MESSAGE_ERROR: NO FRAME (has_frame() == false)\n");
                return;
            }

            if (!bounds.has_value())
            {
                const auto [len, width, goal_width, goal_depth] = environment.field();

                auto left_goal_bounds = std::array<std::complex<double>, 4>{{
                    {-len / 2 - goal_depth, goal_width / 2},
                    {-len / 2, goal_width / 2},
                    {-len / 2, -goal_width / 2},
                    {-len / 2 - goal_depth, -goal_width / 2},
                }};
                // Mirror of the left_goal_bounds, thats why it's out of order.
                auto right_goal_bounds = std::array{
                    left_goal_bounds[2] * -1.0,
                    left_goal_bounds[3] * -1.0,
                    left_goal_bounds[0] * -1.0,
                    left_goal_bounds[1] * -1.0,
                };

                auto field_bounds = std::array<std::complex<double>, 4>{{
                    {-len / 2, width / 2},
                    {len / 2, width / 2},
                    {len / 2, -width / 2},
                    {-len / 2, -width / 2},
                }};

                bounds = {
                    left_goal_bounds,
                    right_goal_bounds,
                    field_bounds,
                };

                strategy.setBounds(bounds.value());
            }

            fira_message::sim_to_ref::Packet packet;

            if (game_on)
            {
                auto [blue_robots, yellow_robots, ball] = environment.frame();
                auto [ball_x, ball_y, ball_z, ball_vx, ball_vy, ball_vz] = ball;


                auto& allied_team = is_yellow ? yellow_robots : blue_robots;

                auto& enemy_team  = is_yellow ? blue_robots   : yellow_robots;


                std::vector<pinguim::vsss::point> pb_enemies = {{
                    {enemy_team[0].x(), enemy_team[0].y()},
                    {enemy_team[1].x(), enemy_team[1].y()},
                    {enemy_team[2].x(), enemy_team[2].y()}
                }};

                for (const auto &robot : allied_team)
                {
                    pinguim::vsss::robot pb_robot;
                    pinguim::vsss::ball  pb_ball;

                    pb_robot.id = toe << robot.robot_id();
                    pb_robot.position = {robot.x(), robot.y()};
                    pb_robot.orientation = robot.orientation();
                    pb_ball.position = pinguim::vsss::point{ball.x(), ball.y()};
                    pb_ball.velocity = pinguim::vsss::point{ball.vx(), ball.vy()};

                    for (const auto &other_robot : allied_team) {
                        if (other_robot.robot_id() != tou << pinguim::vsss::Roles::ATTACKER) {
                            pb_enemies.push_back({other_robot.x(), other_robot.y()});
                        }
                    }

                    auto [new_point, rotation] = strategy.actions(pb_robot,
                                                                  pb_ball,
                                                                  pb_enemies);

                    auto [left, right] = pinguim::vsss::to_pair( pinguim::vsss::control::generate_vels(pb_robot, new_point, rotation));

                    auto command = packet.mutable_cmd()->add_robot_commands();
                    command->set_id(robot.robot_id());
                    command->set_yellowteam(is_yellow);
                    command->set_wheel_left(left);
                    command->set_wheel_right(right);
                    VSSS.simulator_send(packet);

                }

            }
            else
            {
                for (int i = 0; i < 3; ++i)
                {
                    auto command = packet.mutable_cmd()->add_robot_commands();
                    command->set_id( toe << i );
                    command->set_yellowteam(is_yellow);
                    command->set_wheel_left(0.0);
                    command->set_wheel_right(0.0);
                    VSSS.simulator_send(packet);
                }
            }
        },
        /* referee_in_params= */ {args.referee_address,   args.referee_port},
        /* replacer_out_params= */ {args.referee_address, args.rep_port},
        /* on_referee_receive= */ [&](auto command) {
            auto [foul, teamcolor, foul_quadrant, timestamp, game_half] = command;
            // foul => {
            //    VSSRef::Foul::FREE_KICK, ...::PENALTY_KICK, ...::GOAL_KICK,
            //    ...::FREE_BALL, ...::KICKOFF , ...::STOP , ...::GAME_ON}
            // color => {
            //    VSSRef::Color::BLUE, ...::YELLOW, ...::NONE (que ?)}
            // foul_quadrant => {
            //     VSSRef::Quadrant::NO_QUADRANT, ...::QUADRANT_1,
            //     ...::QUADRANT_2, ...::QUADRANT_3, ...::QUADRANT_4}
            // timestamp => double
            // game_half => {
            //    VSSRef::Half::NO_HALF, ...::FIRST_HALF, ...::SECOND_HALF}

            switch (foul)
            {
            case VSSRef::Foul::FREE_BALL:
                break;
            case VSSRef::Foul::FREE_KICK:
                break;
            case VSSRef::Foul::GOAL_KICK:
                break;
            case VSSRef::Foul::PENALTY_KICK:
                break;
            case VSSRef::Foul::KICKOFF:
                break;
            case VSSRef::Foul::STOP:
                game_on = false;

                break;
            case VSSRef::Foul::GAME_ON:
                game_on = true;
                break;
            default:
                break;
            }

            switch (teamcolor)
            {

            case VSSRef::Color::NONE:
                break;
            case VSSRef::Color::BLUE:
                break;
            case VSSRef::Color::YELLOW:
                break;
            default:
                //Cor não identificada
                break;
            }

            switch (foul_quadrant)
            {
            case VSSRef::Quadrant::NO_QUADRANT:
                break;
            case VSSRef::Quadrant::QUADRANT_1:
                break;
            case VSSRef::Quadrant::QUADRANT_2:
                break;
            case VSSRef::Quadrant::QUADRANT_3:
                break;
            case VSSRef::Quadrant::QUADRANT_4:
                break;
            default:
                break;
            }

            //Estrutura pra idenficar a falta e reposicionar
            //VSSRef::team_to_ref::VSSRef_Placement placement;
            //VSSS.replacer_send(placement)
        }};

    // Start the event loop (needed to be able to send and receive messages).
    app.exec();

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
