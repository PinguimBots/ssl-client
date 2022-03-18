#include "pinguim/app/subsystems/misc/imgui_debug.hpp"

#include "pinguim/app/subsystems/registrar.hpp"

#include <imgui.h>

PINGUIM_APP_REGISTER_MISC_SUBSYSTEM(pinguim::app::subsystems::misc::imgui_debug, "Imgui debug");

auto pinguim::app::subsystems::misc::imgui_debug::loop([[maybe_unused]] float delta_seconds) -> void
{
    ImGui::BeginMainMenuBar();

    if(ImGui::BeginMenu("[DEBUG] ImGui"))
    {
        if( ImGui::MenuItem("Metrics", nullptr, show_metrics) )
        { show_metrics = !show_metrics; }
        if( ImGui::MenuItem("Demo", nullptr, show_demo) )
        { show_demo = !show_demo; }
        if( ImGui::MenuItem("Stack tool", nullptr, show_stack_tool) )
        { show_stack_tool = !show_stack_tool; }
        if( ImGui::MenuItem("About", nullptr, show_about) )
        { show_about = !show_about; }

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    if(show_metrics)    { ImGui::ShowMetricsWindow(&show_metrics); }
    if(show_demo)       { ImGui::ShowDemoWindow(&show_demo); }
    if(show_stack_tool) { ImGui::ShowStackToolWindow(&show_stack_tool); }
    if(show_about)      { ImGui::ShowAboutWindow(&show_about); }
}
