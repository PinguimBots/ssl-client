#include <fmt/core.h>

#include <complex>
#include <csignal>
#include <cstdlib> // For std::exit;

// Networking stuff.
#include <pinguim/simproto/all>

#include "pinguim/vsss/net/decoders/sim.hpp"
#include "pinguim/vsss/net/multicast_udp_receiver.hpp"
#include "pinguim/vsss/net/udp_sender.hpp"
#include "pinguim/vsss/net/encoders/sim.hpp"

// Gui stuff.
#include <imgui.h>

#include "pinguim/imgui/plumber.hpp"
#include "pinguim/app/ui.hpp"

// Strategy stuff.
#include "pinguim/vsss/strategy.hpp"
#include "pinguim/vsss/control.hpp"

// Misc.
#include "pinguim/vsss/firasim/tuplified_proto.hpp"
#include "pinguim/app/cmdline.hpp"
#include "pinguim/chrono.hpp"
#include "pinguim/cvt.hpp"

#include <opencv2/imgcodecs.hpp>
#include "pinguim/imgui/img.hpp"

#include "pinguim/utils.hpp"

namespace cvt = pinguim::cvt

auto do_strategy(auto& is_yellow, auto& game_on, auto& bounds, auto& strategy, auto& lerp_coords, auto& env_packet, auto& out_packet, auto& strategy_vis, auto& sim_sender)
{
    strategy_vis = cv::Scalar{0};

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
    {
        const auto [topl_x, topl_y] = lerp_coords(-env_packet.field().length()/2, env_packet.field().width()/2);
        const auto [botr_x, botr_y] = lerp_coords(env_packet.field().length()/2, -env_packet.field().width()/2);
        cv::rectangle(strategy_vis, {topl_x, topl_y}, {botr_x, botr_y}, {255, 255, 255}, 4);
    }

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
        for(const auto& robot:enemy_team) {
            const auto [x, y] = lerp_coords(robot.x(), robot.y());
            cv::circle(strategy_vis, {x, y}, 8, {0, 0, 255}, cv::FILLED);
        }
        const auto [pixel_ball_x, pixel_ball_y] = lerp_coords(ball_x, ball_y);
        cv::circle(strategy_vis, {pixel_ball_x, pixel_ball_y}, 3, {0, 255, 0}, cv::FILLED);

        for (const auto &robot : allied_team)
        {
            pinguim::vsss::robot pb_robot;
            pinguim::vsss::ball  pb_ball;

            pb_robot.id = robot.robot_id() * cvt::toe;
            pb_robot.position = {robot.x(), robot.y()};
            const auto [lerped_x, lerped_y] = lerp_coords(robot.x(), robot.y());
            cv::circle(strategy_vis, {lerped_x, lerped_y}, 8, {255, 0, 255}, cv::FILLED);
            pb_robot.orientation = robot.orientation();
            pb_ball.position = pinguim::vsss::point{ball.x(), ball.y()};
            pb_ball.velocity = pinguim::vsss::point{ball.vx(), ball.vy()};

            for (const auto &other_robot : allied_team) {
                if (other_robot.robot_id() != cvt::tou * pinguim::vsss::Roles::ATTACKER) {
                    pb_enemies.push_back({other_robot.x(), other_robot.y()});
                }
            }

            auto [new_point, rotation] = strategy.actions(pb_robot,
                                                            pb_ball,
                                                            pb_enemies);

            auto [left, right] = pinguim::vsss::to_pair( pinguim::vsss::control::generate_vels(pb_robot, new_point, rotation));

            auto command = out_packet.mutable_cmd()->add_robot_commands();
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
            auto command = out_packet.mutable_cmd()->add_robot_commands();
            command->set_id( i * cvt::toe );
            command->set_yellowteam(is_yellow);
            command->set_wheel_left(0.0);
            command->set_wheel_right(0.0);
        }
    }

    sim_sender.send(out_packet);
}

auto read_frame(auto& cap, auto& mat)
{
    if(!cap.isOpened()) {
        fmt::print("ERROR: read_frame called with unopened capture");
        std::exit(1);
    }

    // Try to go to the beginning of the capture file.
    if(!cap.read(mat)) {
        cap.set(cv::CAP_PROP_POS_AVI_RATIO, 0);
        cap.read(mat);
    }
}

auto do_vision()
{
    ImGui::Begin("Vision");

    static auto raw_video = cv::VideoCapture("tests/testfiles/video_robos.webm");
    static auto raw_mat = cv::Mat();
    read_frame(raw_video, raw_mat);
    static auto raw_img = pinguim::imgui::img{raw_mat};
    raw_img = {raw_mat};

    ImGui::Image(raw_img);

    static auto preprocess_mat = cv::Mat();
    cv::GaussianBlur(raw_mat, preprocess_mat, cv::Size(5, 5), 0, 0);
    static auto preprocess_img = pinguim::imgui::img{preprocess_mat};
    preprocess_img = {preprocess_mat};

    ImGui::SameLine();
    ImGui::Image(preprocess_img);

    cv::cvtColor(preprocess_mat, preprocess_mat, cv::COLOR_BGR2HSV);

    ImGui::End();
}

namespace ui = pinguim::app::ui;

auto quit = false;

#include "pinguim/geometry.hpp"

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

    const auto args = pinguim::app::cmdline::parse_argv_or_abort(
        argc,
        const_cast<const char**>(argv)
    );

    const auto is_yellow = args.team == "yellow";
    auto bounds = std::optional<pinguim::vsss::field_geometry>{};
    auto strategy = pinguim::vsss::Strategy();
    strategy.setTeam(is_yellow);
    bool game_on = true;

    using receiver_t = pinguim::vsss::net::multicast_udp_receiver<512>;
    using sender_t   = pinguim::vsss::net::udp_sender;
    auto env_receiver     = receiver_t{args.in_address,      args.in_port};
    auto referee_receiver = receiver_t{args.referee_address, args.referee_port};
    auto sim_sender       = sender_t{args.out_address, args.out_port};

    auto env_packet     = fira_message::sim_to_ref::Environment{};
    auto referee_packet = VSSRef::ref_to_team::VSSRef_Command{};
    auto out_packet     = fira_message::sim_to_ref::Packet{};

    auto mario = pinguim::imgui::make_plumber().value();

    auto windows = ui::windows{};

    auto strategy_vis = cv::Mat(300, 600, CV_8UC3);
    const auto lerp_coords = [&strategy_vis, &env_packet](auto x_irl, auto y_irl) -> std::tuple<int, int> {
        // val =  minOutRange + (maxOutRange - minOutRange) * (inValue - minInRange) / (maxInRange - minInRange);
        const auto total_d = env_packet.field().length() + 2*env_packet.field().goal_depth();
        const auto total_w = env_packet.field().width();
        auto x = strategy_vis.cols * (x_irl + total_d/2)/total_d;
        auto y = strategy_vis.rows * (y_irl + total_w/2)/total_w;

        return {x * cvt::toe, y * cvt::toe};
    };

    while(!quit){
        quit = mario.handle_event();
        mario.begin_frame();

        auto new_env     = env_receiver.sync( env_packet );
        auto new_referee = referee_receiver.sync( referee_packet );
        out_packet.Clear();

        ui::draw_menubar(windows);
        if(windows.env_packet)
        {
            ui::draw_env_packet_window(
                windows,
                args.in_address,
                args.in_port,
                env_packet
            );
        }
        if(windows.demo) { ui::draw_demo_window(); }
        if(windows.lena) { ui::draw_lena_window(windows); }

        if(new_env) {
            do_strategy(
                is_yellow,
                game_on,
                bounds,
                strategy,
                lerp_coords,
                env_packet,
                out_packet,
                strategy_vis,
                sim_sender
            );
        }

        do_vision();

        static auto strategy_vis_img = pinguim::imgui::img{};
        strategy_vis_img = {strategy_vis};
        ImGui::Image(strategy_vis_img);

        mario.draw_frame();
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
