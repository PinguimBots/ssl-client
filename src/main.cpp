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

    auto controle = pbts::Control{};

    auto isYellow = args["--team"].asString() == "yellow";

    fira_message::sim_to_ref::Environment packet;
    while(true) {
        if (client.receive(packet)) {
            /// TODO: Make sure it's necessary to verify has_frame() and has_field().
            if (packet.has_frame()) {
                auto [blue_robots, yellow_robots, ball] = packet.frame();
                auto [ball_x, ball_y, ball_z, ball_vx, ball_vy, ball_vz] = ball;

                for(const auto& robot : blue_robots) {
                    auto [left, right] = controle.generateVels(robot, ball);

                    grSim_client.sendCommand(left, right, robot.robot_id() + 1, isYellow);
                }

                for(const auto& robot : yellow_robots) {
                }
            }

            //see if packet contains geometry data:
            if (packet.has_field()){
                using namespace std::complex_literals;

                const auto [len, width, goal_width, goal_depth] = packet.field();

                auto goal_a = std::complex(-len/2 - goal_depth, goal_width/2);
                auto goal_b = std::complex(-len/2, goal_width/2);
                auto goal_c = goal_a * -1i;
                auto goal_d = goal_b * -1i;
                auto left_goal_bounds = std::array{
                    goal_a, goal_b,
                    goal_c, goal_d
                };
                // Mirror of the left_goal_bounds, thats why it's out of order.
                auto right_goal_bounds = std::array{
                    left_goal_bounds[1] * -1., left_goal_bounds[0] * -1.,
                    left_goal_bounds[3] * -1., left_goal_bounds[2] * -1.,
                };

                auto field_a = std::complex(-len/2, width/2);
                auto field_b = std::complex(len/2, width/2);
                auto field_c = field_a * -1i;
                auto field_d = field_b * -1i;
                auto field_bounds = std::array{
                    field_a, field_b,
                    field_c, field_d
                };
            }
        }
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
