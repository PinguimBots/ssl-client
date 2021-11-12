#include <fmt/core.h>

#include <complex>
#include <csignal>
#include <cstdlib> // For std::exit;

// Networking stuff.
#include <simproto.hpp>

#include "pinguim/vsss/net/decoders/sim.hpp"
#include "pinguim/vsss/net/multicast_udp_receiver.hpp"
#include "pinguim/vsss/net/udp_sender.hpp"
#include "pinguim/vsss/net/encoders/sim.hpp"

// Gui stuff.
#include <imgui.h>

#include "pinguim/imgui/widgets/group_panel.hpp"
#include "pinguim/imgui/plumber.hpp"
#include "pinguim/imgui/fmt.hpp"

// Strategy stuff.
#include "pinguim/vsss/strategy.hpp"
#include "pinguim/vsss/control.hpp"

// Misc.
#include "pinguim/vsss/firasim/tuplified_proto.hpp"
#include "pinguim/app/cmdline.hpp"
#include "pinguim/cvt.hpp"

#include <opencv2/imgcodecs.hpp>
#include "pinguim/imgui/img.hpp"

using pinguim::cvt::toe;
using pinguim::cvt::tou;

auto quit = false;

#include "pinguim/utils.hpp"

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::signal(SIGINT, []([[maybe_unused]] int signal) {
        fmt::print("SIGINT Received, Closing\n");
        quit = true;
    });
    std::signal(SIGABRT, []([[maybe_unused]] int signal) {
        fmt::print("SIGABRT Received. Something went bad!\n");
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

    auto env_receiver     = pinguim::vsss::net::multicast_udp_receiver{args.in_address,      args.in_port};
    auto referee_receiver = pinguim::vsss::net::multicast_udp_receiver{args.referee_address, args.referee_port};
    auto sim_sender       = pinguim::vsss::net::udp_sender{args.out_address, args.out_port};

    auto env_packet     = fira_message::sim_to_ref::Environment{};
    auto referee_packet = VSSRef::ref_to_team::VSSRef_Command{};

    auto mario = pinguim::imgui::make_plumber().value();
    // Make sure to only try to do this once OpenGl is initialized.
    auto lena = pinguim::imgui::img{cv::imread("tests/testfiles/lena.png")};

    auto show_env_packet_window = true;
    auto show_referee_packet_window = false;
    auto show_out_packet_window = true;
    auto show_demo_window = false;
    auto show_lena_window = false;

    while(!quit){
        quit = mario.handle_event();
        mario.begin_frame();

        auto new_env = env_receiver.sync( env_packet );
        auto new_referee = referee_receiver.sync( referee_packet );

        ImGui::BeginMainMenuBar();
        if(ImGui::BeginMenu("DEBUG")) {
            ImGui::MenuItem("env packet window",     nullptr, &show_env_packet_window);
            ImGui::MenuItem("referee packet window", nullptr, &show_referee_packet_window);
            ImGui::MenuItem("out packet window",     nullptr, &show_out_packet_window);
            ImGui::MenuItem("demo window",           nullptr, &show_demo_window);
            ImGui::MenuItem("lena",                  nullptr, &show_lena_window);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        if(show_env_packet_window && ImGui::Begin(fmt::format("Environment Packet (received from {}:{})", args.in_address, args.in_port).c_str(), &show_env_packet_window))
        {
            ImGui::BeginGroup();
            {
                ImGui::ValueT("step", env_packet.step());
                ImGui::ValueT("goals_blue",   env_packet.goals_blue());
                ImGui::ValueT("goals_yellow", env_packet.goals_yellow());
            }
            ImGui::EndGroup();
            ImGui::SameLine(0.f, 103.f);
            ImGui::BeginGroupPanel("field");
            {
                const auto& field = env_packet.field();
                ImGui::ValueT(" w",      field.width(),      ":.2f");
                ImGui::SameLine(0.f, 20.f);
                ImGui::ValueT("goal_w",  field.goal_width(), ":.2f");

                ImGui::ValueT(" l",      field.length(),     ":.2f");
                ImGui::SameLine(0.f, 20.f);
                ImGui::ValueT("goal_d",  field.goal_depth(), ":.2f");
            }
            ImGui::EndGroupPanel();

            ImGui::BeginGroupPanel("frame");
            {
                ImGui::BeginGroupPanel("ball");
                {
                    const auto& ball = env_packet.frame().ball();
                    ImGui::ValueT(" x", ball.x(), ": .2f");
                    ImGui::SameLine(0.f, 90.f);
                    ImGui::ValueT("y",  ball.y(), ": .2f");
                    ImGui::SameLine(0.f, 90.f);
                    ImGui::ValueT("z",  ball.z(), ": .2f");
                }
                ImGui::EndGroupPanel();

                constexpr const char* robot_names[] = {"zero", "uno", "dos"};
                ImGui::BeginGroupPanel("yellow team");
                for(const auto& robot : env_packet.frame().robots_yellow())
                {
                    ImGui::BeginGroupPanel( fmt::format("roboto {}", robot_names[robot.robot_id()]).c_str() );
                    ImGui::ValueT(" x", robot.x(),            ": .2f");
                    ImGui::SameLine();
                    ImGui::ValueT("vx", robot.vx(),           ": .2f");

                    ImGui::ValueT(" y", robot.y(),            ": .2f");
                    ImGui::SameLine();
                    ImGui::ValueT("vy", robot.vy(),           ": .2f");

                    ImGui::ValueT(" o", robot.orientation(),  ": .2f");
                    ImGui::SameLine();
                    ImGui::ValueT("vo", robot.vorientation(), ": .2f");
                    ImGui::EndGroupPanel();
                }
                ImGui::EndGroupPanel();
                ImGui::SameLine();
                ImGui::BeginGroupPanel("blue team");
                for(const auto& robot: env_packet.frame().robots_blue())
                {
                    ImGui::BeginGroupPanel( fmt::format("roboto {}", robot_names[robot.robot_id()]).c_str() );
                    ImGui::ValueT(" x", robot.x(),            ": .2f");
                    ImGui::SameLine();
                    ImGui::ValueT("vx", robot.vx(),           ": .2f");

                    ImGui::ValueT(" y", robot.y(),            ": .2f");
                    ImGui::SameLine();
                    ImGui::ValueT("vy", robot.vy(),           ": .2f");

                    ImGui::ValueT(" o", robot.orientation(),  ": .2f");
                    ImGui::SameLine();
                    ImGui::ValueT("vo", robot.vorientation(), ": .2f");
                    ImGui::EndGroupPanel();
                }
                ImGui::EndGroupPanel();
            }
            ImGui::EndGroupPanel();

            ImGui::End();
        } else if (show_env_packet_window) { ImGui::End(); }

        if(show_referee_packet_window && ImGui::Begin(fmt::format("Referee Packet (received from {}:{})", args.referee_address, args.referee_port).c_str(), &show_referee_packet_window))
        {
            ///TODO: implement
            ImGui::End();
        } else if (show_referee_packet_window) { ImGui::End(); }

        if(show_demo_window) { ImGui::ShowDemoWindow(); }

        if(show_lena_window && ImGui::Begin("Lena (OpenGL/OpenCV test)", &show_lena_window))
        {
            ImGui::Image(lena);
            ImGui::End();
        } else if(show_lena_window) { ImGui::End(); }

        if(new_env) {
            // Strategy
            if (!bounds.has_value())
            {
                const auto [len, width, goal_width, goal_depth] = env_packet.field();

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
                auto [blue_robots, yellow_robots, ball] = env_packet.frame();
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
                }
            }

            sim_sender.send(packet);
        }

        if(show_out_packet_window && ImGui::Begin(fmt::format("Out Packet (sent to {}:{})", args.out_address, args.out_port).c_str(), &show_out_packet_window))
        {
            ///TODO: implement
            ImGui::End();
        } else if (show_out_packet_window) { ImGui::End(); }

        mario.draw_frame();
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;

// Commented out for now.
// Working on converting to asio.
#if 0

    pinguim::vsss::simulator_connection VSSS{
        {args.in_address,  args.in_port},
        {args.out_address, args.out_port},
        /* on_simulator_receive= */ [&](auto environment) {
            /// TODO: measure timing

            /// TODO: Make sure it's necessary to verify has_frame() and has_field().

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

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
#endif
}
