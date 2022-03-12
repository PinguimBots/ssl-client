#include "pinguim/app/subsystems/manager.hpp"

#include "pinguim/app/subsystems/all"
#include "pinguim/cvt.hpp"

#include <imgui.h>

namespace i = pinguim::app::subsystems::input;
namespace l = pinguim::app::subsystems::logic;
namespace o = pinguim::app::subsystems::output;

auto pinguim::app::subsystems::manager::draw_selector_ui(float delta_seconds) -> void
{
    namespace ImGui = ::ImGui;

    // Just for automating some boilerplate.
    #define MENU_ITEM(namestr, enum_var, enum_val, enum_none_val, setter, set_type) \
        if( ImGui::MenuItem( namestr, nullptr, enum_var == enum_val) )              \
        {                                                                           \
            if( enum_var != enum_val ) {                                            \
                setter(new set_type);                                               \
                enum_var = enum_val;                                                \
            } else {                                                                \
                setter(nullptr);                                                    \
                enum_var = enum_none_val;                                           \
            }                                                                       \
        }

    ImGui::BeginMainMenuBar();

    ImGui::Text("%.2f MS", cvt::to<double> * delta_seconds * 1000);

    if(ImGui::BeginMenu("Subsystems"))
    {
        if(ImGui::BeginMenu("Input"))
        {
            MENU_ITEM("Firasim", input_t, input_type::firasim, input_type::none, input, i::firasim);
            MENU_ITEM("Vision",  input_t, input_type::vision,  input_type::none, input, i::vision);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Logic"))
        {
            MENU_ITEM("Direct Control", logic_t, logic_type::direct_control, logic_type::none, logic, l::direct_control);
            MENU_ITEM("Strategy",       logic_t, logic_type::strategy,       logic_type::none, logic, l::strategy);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Output"))
        {
            MENU_ITEM("Firasim", output_t, output_type::firasim, output_type::none, output, o::firasim);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    #undef MENU_ITEM
}

auto pinguim::app::subsystems::manager::input(pinguim::app::input_subsystem* ptr) -> void
{ _input.reset(ptr); }
auto pinguim::app::subsystems::manager::logic(pinguim::app::logic_subsystem* ptr) -> void
{ _logic.reset(ptr); }
auto pinguim::app::subsystems::manager::output(pinguim::app::output_subsystem* ptr) -> void
{ _output.reset(ptr); }

auto pinguim::app::subsystems::manager::update_gameinfo(game_info& gi, float delta_seconds) -> bool
{
    if(_input) return _input->update_gameinfo(gi, delta_seconds);
    return false;
}
auto pinguim::app::subsystems::manager::run_logic(game_info const& gi, commands& cmds, float delta_seconds) -> bool
{
    if(_logic) return _logic->run_logic(gi, cmds, delta_seconds);
    return false;
}
auto pinguim::app::subsystems::manager::transmit(commands const& cmds, float delta_seconds) -> bool
{
    if(_output) return _output->transmit(cmds, delta_seconds);
    return false;
}
