#include "pinguim/app/ui.hpp"

#include "pinguim/imgui/widgets/group_panel.hpp"
#include "pinguim/imgui/fmt.hpp"
#include "pinguim/imgui/img.hpp"
#include "pinguim/aliases.hpp"

#include <simproto.hpp>

#include <imgui.h>

auto pinguim::app::ui::draw_menubar(windows& w) -> void
{
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("DEBUG"))
    {
        ImGui::MenuItem("env packet window",     nullptr, &w.env_packet);
        ImGui::MenuItem("referee packet window", nullptr, &w.referee_packet);
        ImGui::MenuItem("out packet window",     nullptr, &w.out_packet);
        ImGui::MenuItem("demo window",           nullptr, &w.demo);
        ImGui::MenuItem("lena",                  nullptr, &w.lena);
        ImGui::MenuItem("opencv",                nullptr, &w.opencv);
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

auto pinguim::app::ui::draw_env_packet_window(
    windows& w,
    std::string_view in_address,
    u16 in_port,
    const fira_message::sim_to_ref::Environment& env_packet
) -> void
{
    if(
        ImGui::Begin(
            fmt::format(
                "Environment Packet (received from {}:{})", in_address, in_port
            ).c_str(),
            &w.env_packet
        )
    )
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
    }

    ImGui::End();
}

auto pinguim::app::ui::draw_demo_window() -> void
{ ImGui::ShowDemoWindow(); }

auto pinguim::app::ui::draw_lena_window(windows& w) -> void
{
    static auto lena = pinguim::imgui::img{cv::imread("tests/testfiles/lena.png")};

    if(ImGui::Begin("Lena (OpenGL/OpenCV test)", &w.lena))
    { ImGui::Image(lena); }
    ImGui::End();
}
