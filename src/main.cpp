#include <complex>
#include <cstdint>

#include <fmt/core.h>

#include <docopt.h>

#include "net/robocup_ssl_client.h"
#include "net/grSim_client.h"

#include "pb/command.pb.h"
#include "pb/common.pb.h"
#include "pb/packet.pb.h"
#include "pb/replacement.pb.h"

#include "pbts/control.h"
#include "pbts/qol.hpp"

static const constexpr char usage[] = R"(pbssl ver 0.0.

    Usage:
        pbssl [(--team=TEAM | -t=TEAM)] [--in-port=INPORT] [--in-address=INADDR] [--out-port=OUTPORT] [--out-address=OUTADDR]

    Options:
        -h --help              Show this screen.
        --version              Show version.
        --team TEAM, -t TEAM   asdf [default: blue].
        --in-port INPORT       asdf [default: 10020].
        --in-address INADDR    asdf [default: 224.0.0.0].
        --out-port OUTPORT     asdf [default: 20011].
        --out-address OUTADDR  asdf [default: 127.0.0.1].
)"; 

int main(int argc, char* argv[]){
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    auto args = docopt::docopt(
        usage,
        {argv + 1, argv + argc},
        true,            // show help if requested
        "pbssl ver 0.0"  // version string
    );
    /// TODO: validate.

    fmt::print("using args:\n");
    for(const auto& [flag, value] : args) {
        fmt::print("\t{} = {}\n", flag, value.asString());
    }

    auto client = RoboCupSSLClient{
        static_cast<std::uint16_t>( args["--in-port"].asLong() ),
        args["--in-address"].asString()
    };
    client.open(false);

    auto grSim_client = GrSim_Client{
        static_cast<std::uint16_t>(args["--out-port"].asLong() ),
        args["--out-address"].asString()
    };

    //auto strategy = pbts::Strategy{};

    auto is_yellow = args["--team"].asString() == "yellow";

    fira_message::sim_to_ref::Environment environment;
    while(true) {
        if (client.receive(environment)) {
            /// TODO: Make sure it's necessary to verify has_frame() and has_field().
            //see if environment contains geometry data:
            if (environment.has_field()){
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
                );*/
            }
    
            if (environment.has_frame()) {
                auto [blue_robots, yellow_robots, ball] = environment.frame();
                auto [ball_x, ball_y, ball_z, ball_vx, ball_vy, ball_vz] = ball;

                auto& allied_team = is_yellow ? yellow_robots : blue_robots;
                auto target_goal  = is_yellow
                    ? pbts::Strategy::goal_bound_type::left
                    : pbts::Strategy::goal_bound_type::right;
                auto& enemy_team  = is_yellow ? blue_robots   : yellow_robots;

                //strategy.tick(allied_team, enemy_team);

                for(const auto& robot : blue_robots) {
                    /*auto [left, right] = controle.generateVels(robot, ball);

                    grSim_client.sendCommand(left, right, robot.robot_id() + 1, is_yellow);
                    */
                }
            }
        }
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
