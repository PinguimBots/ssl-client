#include <docopt.h>

#include <QCoreApplication>

#include <fmt/core.h>
#include <fmt/chrono.h>

#include <complex>
#include <csignal>

#include "pbts/simulator_connection.hpp"
#include "pbts/strategy.h"
#include "pbts/control.h"
#include "pbts/parse.hpp"
#include "pbts/qol.hpp"

static const constexpr char usage[] = R"(pbssl ver 0.0.

    Usage:
        pbssl [(--team=TEAM | -t=TEAM)] [--in-port=INPORT] [--in-address=INADDR] [--out-port=OUTPORT] [--out-address=OUTADDR]

    Options:
        -h --help              Show this screen.
        --version              Show version.
        --team TEAM, -t TEAM   asdf [default: blue].
        --in-port INPORT       Multicast group port    used to listen to fira_sim::sim_to_ref::Environment [default: 10020].
        --in-address INADDR    Multicast group address used to listen to fira_sim::sim_to_ref::Environment [default: 224.0.0.0].
        --out-port OUTPORT     Port    used to send fira_sim::sim_to_ref::Packet [default: 20011].
        --out-address OUTADDR  Address used to send fira_sim::sim_to_ref::Packet [default: 127.0.0.1].
)"; 

int main(int argc, char* argv[]){
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // We dont want Qt to hijack the command line arguments 
    // so we'll just lie saying that there were none.
    // Also, why doesn't it take an int but an int&, nonsense.
    auto fake_arg_count = 1;
    auto app = QCoreApplication{fake_arg_count, argv};
    std::signal(SIGINT, [](int signal){
        fmt::print("SIGINT Received, calling QCoreApplication::exit()\n");
        QCoreApplication::exit(0);
    });

    auto args = docopt::docopt(
        usage,
        {argv + 1, argv + argc},
        true,            // show help if requested
        "pbssl ver 0.0"  // version string
    );

    auto maybe_in_port  = pbts::parse::port( args["--in-port"].asString() );
    if(!maybe_in_port)
        {fmt::print("PARSE_ERROR: invalid in-port\n");    return 1;}
    auto in_addr_valid  = pbts::parse::ipv4( args["--in-address"].asString() );
    if(!in_addr_valid)
        {fmt::print("PARSE_ERROR: invalid in-address\n"); return 1;}
    auto maybe_out_port = pbts::parse::port( args["--out-port"].asString() );
    if(!maybe_out_port)
        {fmt::print("PARSE_ERROR: invalid out-port\n");   return 1;}
    auto out_addr_valid = pbts::parse::ipv4( args["--out-address"].asString() );
    if(!out_addr_valid)
        {fmt::print("PARSE_ERROR: invalid out-address\n");return 1;}

    const auto in_addr = args["--in-address"].asString();
    const auto in_port = static_cast<std::uint16_t>(maybe_in_port.value());
    const auto out_addr = args["--out-address"].asString();
    const auto out_port = static_cast<std::uint16_t>(maybe_out_port.value());

    const auto is_yellow = args["--team"].asString() == "yellow";

    auto controle = pbts::Control{};
    //auto strategy = pbts::Strategy{};

    pbts::simulator_connection simulator{
        {in_addr, in_port},
        {out_addr, out_port},
        /* on_receive= */ [&](auto environment) {
            /// TODO: measure timing

            /// TODO: Make sure it's necessary to verify has_frame() and has_field().
            if(!environment.has_field()) {
                fmt::print("MESSAGE_ERROR: NO FIELD (has_field() == false)\n");
                return;
            } else if(!environment.has_frame()) {
                fmt::print("MESSAGE_ERROR: NO FRAME (has_frame() == false)\n");
                return;
            }

            const auto [len, width, goal_width, goal_depth] = environment.field();

            auto left_goal_bounds = std::array< std::complex<double>, 4>{{
                {-len/2 - goal_depth, goal_width/2}, {-len/2, goal_width/2},
                {-len/2 - goal_depth, -goal_width/2}, {-len/2, -goal_width/2},
            }};
            // Mirror of the left_goal_bounds, thats why it's out of order.
            auto right_goal_bounds = std::array{
                left_goal_bounds[1] * -1.0, left_goal_bounds[0] * -1.0,
                left_goal_bounds[3] * -1.0, left_goal_bounds[2] * -1.0,
            };
    
            auto field_bounds = std::array< std::complex<double>, 4>{{
                {-len/2, width/2}, {len/2, width/2},
                {-len/2, -width/2}, {len/2, -width/2},
            }};

            /*
            strategy.set_bounds(
                left_goal_bounds,
                right_goal_bounds,
                field_bounds
            );
            */

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

            for(const auto& robot : blue_robots) {
                auto [left, right] = controle.generateVels(robot, ball);

                /*fmt::print(
                    "Robo: {}\n"
                    "\tVel Left: {} | Vel Right: {}\n",
                    robot.robot_id(), left, right);
                */

                auto command = packet.mutable_cmd()->add_robot_commands();
                command->set_id(robot.robot_id());
                command->set_yellowteam(is_yellow);
                command->set_wheel_left(left);
                command->set_wheel_right(right);
            }

            simulator.send(packet);
        }
    };

    // Start the event loop (needed to be able to send and receive messages).
    app.exec();

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
