#include <docopt.h>

#include <QCoreApplication>

#include <fmt/core.h>

#include <complex>
#include <csignal>

#include "pbts/simulator_connection.hpp"
#include "pbts/strategy.hpp"
#include "pbts/control.hpp"
#include "pbts/parse.hpp"
#include "pbts/qol.hpp"

static const constexpr char usage[] = R"(pbssl ver 0.0.

    Usage:
        pbssl [(--team=TEAM | -t=TEAM)] [--in-port=INPORT] [--in-address=INADDR] [--out-port=OUTPORT] [--out-address=OUTADDR] [--ref-address=ADDREF] [--ref-port=REFPORT] [--rep-port=REPPORT]

    Options:
        -h --help              Show this screen.
        --version              Show version.
        --team TEAM, -t TEAM   Set team color [default: blue].
        --in-port INPORT       Multicast group port    used to listen for fira_sim::sim_to_ref::Environment [default: 10020].
        --in-address INADDR    Multicast group address used to listen for fira_sim::sim_to_ref::Environment [default: 224.0.0.0].
        --out-port OUTPORT     Port    used to send fira_sim::sim_to_ref::Packet [default: 20011].
        --out-address OUTADDR  Address used to send fira_sim::sim_to_ref::Packet [default: 127.0.0.1].
        --ref-address ADDREF   Address used to communicate with the VSSRef [default: 224.0.0.1].
        --ref-port REFPORT     Port to receive information from the Referee [default: 10003].
        --rep-port REPPORT     Port to send replacement information to the Referee [default: 10004].
)";

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // We dont want Qt to hijack the command line arguments
    // so we'll just lie saying that there were none.
    // Also, why doesn't it take an int but an int&, nonsense.
    auto fake_arg_count = 1;
    auto app = QCoreApplication{fake_arg_count, argv};
    std::signal(SIGINT, [](int signal) {
        fmt::print("SIGINT Received, calling QCoreApplication::exit()\n");
        QCoreApplication::exit(0);
    });

    auto args = docopt::docopt(
        usage,
        {argv + 1, argv + argc},
        true,           // show help if requested
        "pbssl ver 0.0" // version string
    );

    auto maybe_in_port = pbts::parse::port(args["--in-port"].asString());
    auto maybe_in_addr = pbts::parse::ipv4(args["--in-address"].asString());
    auto maybe_out_port = pbts::parse::port(args["--out-port"].asString());
    auto maybe_out_addr = pbts::parse::ipv4(args["--out-address"].asString());

    auto maybe_ref_addr = pbts::parse::ipv4(args["--ref-address"].asString());
    auto maybe_ref_port = pbts::parse::port(args["--ref-port"].asString());
    auto maybe_rep_port = pbts::parse::port(args["--rep-port"].asString());

    if (!maybe_in_port)
    {
        fmt::print("PARSE_ERROR: invalid in-port\n");
        return 1;
    }
    if (!maybe_in_addr)
    {
        fmt::print("PARSE_ERROR: invalid in-address\n");
        return 1;
    }
    if (!maybe_out_port)
    {
        fmt::print("PARSE_ERROR: invalid out-port\n");
        return 1;
    }
    if (!maybe_out_addr)
    {
        fmt::print("PARSE_ERROR: invalid out-address\n");
        return 1;
    }
    if (!maybe_ref_addr)
    {
        fmt::print("PARSE_ERROR: invalid ref-address\n");
        return 1;
    }
    if (!maybe_ref_port)
    {
        fmt::print("PARSE_ERROR: invalid ref-port\n");
        return 1;
    }
    if (!maybe_rep_port)
    {
        fmt::print("PARSE_ERROR: invalid rep-port\n");
        return 1;
    }

    const auto in_addr = maybe_in_addr.value();
    const auto in_port = static_cast<std::uint16_t>(maybe_in_port.value());
    const auto out_addr = maybe_out_addr.value();
    const auto out_port = static_cast<std::uint16_t>(maybe_out_port.value());

    const auto ref_addr = maybe_ref_addr.value();
    const auto ref_port = static_cast<std::uint16_t>(maybe_ref_port.value());
    const auto rep_port = static_cast<std::uint16_t>(maybe_rep_port.value());

    const auto is_yellow = args["--team"].asString() == "yellow";

    auto bounds = std::optional<pbts::field_geometry>{};

    auto strategy = pbts::Strategy();

    pbts::simulator_connection VSSS{
        {in_addr, in_port},
        {out_addr, out_port},
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
                    {-len / 2 - goal_depth, -goal_width / 2},
                    {-len / 2, -goal_width / 2},
                }};
                // Mirror of the left_goal_bounds, thats why it's out of order.
                auto right_goal_bounds = std::array{
                    left_goal_bounds[1] * -1.0,
                    left_goal_bounds[0] * -1.0,
                    left_goal_bounds[3] * -1.0,
                    left_goal_bounds[2] * -1.0,
                };

                auto field_bounds = std::array<std::complex<double>, 4>{{
                    {-len / 2, width / 2},
                    {len / 2, width / 2},
                    {-len / 2, -width / 2},
                    {len / 2, -width / 2},
                }};

                bounds = {
                    left_goal_bounds,
                    right_goal_bounds,
                    field_bounds,
                };
            }

            auto [blue_robots, yellow_robots, ball] = environment.frame();
            auto [ball_x, ball_y, ball_z, ball_vx, ball_vy, ball_vz] = ball;

            /*
            auto& allied_team = is_yellow ? yellow_robots : blue_robots;
            auto target_goal  = is_yellow
                ? pbts::Strategy::goal_bound_type::left
                : pbts::Strategy::goal_bound_type::right;
            auto& enemy_team  = is_yellow ? blue_robots   : yellow_robots; 
            strategy.tick(allied_team, enemy_team);
            */

            fira_message::sim_to_ref::Packet packet;
            std::vector<pbts::point> pb_enemies = {{
                {yellow_robots[0].x(), yellow_robots[0].y()},
                {yellow_robots[1].x(), yellow_robots[1].y()},
                {yellow_robots[2].x(), yellow_robots[2].y()}
            }};


            for (const auto &robot : blue_robots)
            {
                pbts::robot pb_robot;
                pbts::ball  pb_ball;
                //pbts::point new_point;
                
                pb_robot.robot_id = robot.robot_id();
                pb_robot.position = {robot.x(), robot.y()};
                pb_robot.orientation = robot.orientation();
                pb_robot.id = robot.robot_id();
                pb_ball.position = pbts::point{ball.x(), ball.y()};
                pb_ball.velocity = pbts::point{ball.vx(), ball.vy()};

                auto [new_point, rotation] = strategy.actions(bounds.value(), pb_robot, pb_ball, pb_enemies, 1);
                auto [left, right] = pbts::to_pair( pbts::control::generate_vels(pb_robot, new_point, rotation ));
                //auto [left, right] = pbts::to_pair(pbts::control::rotate(pb_robot, M_PI));

                auto command = packet.mutable_cmd()->add_robot_commands();
                command->set_id(robot.robot_id());
                command->set_yellowteam(is_yellow);
                command->set_wheel_left(left);
                command->set_wheel_right(right);
            }

<<<<<<< HEAD
             for (const auto &robot : yellow_robots)
            {
                pbts::robot pb_robot;
                pbts::ball  pb_ball;
                //pbts::point new_point;
                
                pb_robot.position = {robot.x(), robot.y()};
                pb_robot.orientation = robot.orientation();
                pb_robot.id = robot.robot_id();
                pb_ball.position = pbts::point{ball.x(), ball.y()};
                pb_ball.velocity = pbts::point{ball.vx(), ball.vy()};

                auto [new_point, rotation] = strategy.actions(bounds.value(), pb_robot, pb_ball, pb_enemies, -1);
                auto [left, right] = pbts::to_pair( pbts::control::generate_vels(pb_robot, new_point, rotation ));
                //auto [left, right] = pbts::to_pair(pbts::control::rotate(pb_robot, M_PI));

                auto command = packet.mutable_cmd()->add_robot_commands();
                command->set_id(robot.robot_id());
                command->set_yellowteam(true);
                command->set_wheel_left(left);
                command->set_wheel_right(right);
            }

            simulator.send(packet);
=======
            VSSS.simulator_send(packet);
        },
        /*referee_in_params=*/{ref_addr, ref_port},
        /*replacer_out_params=*/{ref_addr, rep_port},
        /*on_referee_receive=*/ [&](auto command) {
            // TODO: receber pela linha de comando os parametros do referee e replacer.
>>>>>>> 98606af5d3909317cc9b2520f5071c80289d155a
        }};

    // Start the event loop (needed to be able to send and receive messages).
    app.exec();

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
