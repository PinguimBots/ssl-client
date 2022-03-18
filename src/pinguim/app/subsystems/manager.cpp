#include "pinguim/app/subsystems/manager.hpp"

#include "pinguim/app/subsystems/registrar.hpp"
#include "pinguim/app/subsystems/types.hpp"
#include "pinguim/dont_forget.hpp"
#include "pinguim/cvt.hpp"

#include <imgui.h>

#include <memory> // For std::unique_ptr.
#include <vector>

struct pinguim::app::subsystems::manager::impl
{
    // You can only have 1 input/logic/output subsystem active
    // at a time. But you can have any amount of misc subsystems.

    std::unique_ptr<input_subsystem> input{ nullptr };
    std::unique_ptr<logic_subsystem> logic{ nullptr };
    std::unique_ptr<output_subsystem> output{ nullptr };

    registrar::subsystem_id_t instantiated_input_id{ registrar::invalid_id() };
    registrar::subsystem_id_t instantiated_logic_id{ registrar::invalid_id() };
    registrar::subsystem_id_t instantiated_output_id{ registrar::invalid_id() };

    game_info gi = {};
    commands  c  = {};

    struct misc_subsystem_keeper
    {
        registrar::subsystem_id_t id;
        std::unique_ptr<misc_subsystem> instance;
    };
    std::vector<misc_subsystem_keeper> misc_subsystems;
};

pinguim::app::subsystems::manager::manager() : pimpl{ new impl } {}
pinguim::app::subsystems::manager::~manager() { delete pimpl; }

auto pinguim::app::subsystems::manager::draw_selector_ui(float delta_seconds) -> void
{
    namespace ImGui = ::ImGui;

    auto& i = *pimpl;

    // Just for automating some boilerplate.
    #define MENU_ITEM(namestr, id_var, id_val, id_none_val, setter, set_val) \
        if( ImGui::MenuItem(namestr, nullptr, id_var == id_val) )            \
        {                                                                    \
            if( id_var != id_val ) {                                         \
                setter(set_val);                                             \
                id_var = id_val;                                             \
            } else {                                                         \
                setter(nullptr);                                             \
                id_var = id_none_val;                                        \
            }                                                                \
        }

    #define INPUT_ITEM(name, id, maker)  MENU_ITEM(name, i.instantiated_input_id,  id, registrar::invalid_id(), i.input.reset,  cvt::rc<registrar::input_subsystem_maker>(maker)())
    #define LOGIC_ITEM(name, id, maker)  MENU_ITEM(name, i.instantiated_logic_id,  id, registrar::invalid_id(), i.logic.reset,  cvt::rc<registrar::logic_subsystem_maker>(maker)())
    #define OUTPUT_ITEM(name, id, maker) MENU_ITEM(name, i.instantiated_output_id, id, registrar::invalid_id(), i.output.reset, cvt::rc<registrar::output_subsystem_maker>(maker)())

    ImGui::BeginMainMenuBar();
    PINGUIM_DONT_FORGET( ImGui::EndMainMenuBar() );

    ImGui::Text("%.2f MS", cvt::to<double> * delta_seconds * 1000);

    if(!ImGui::BeginMenu("Subsystems")) { return; }

    for(auto r : registrar::instance())
    {
        auto const submenu_name = [&]{ switch(r.type) {
            case registrar::type_enum::input:  return "Input";
            case registrar::type_enum::logic:  return "Logic";
            case registrar::type_enum::output: return "Output";
            case registrar::type_enum::misc:   return "Misc";
        }}();

        if(!ImGui::BeginMenu(submenu_name)) { continue; }

        if(r.type == registrar::type_enum::input)
        { INPUT_ITEM(r.name, r.id, r.maker); }
        else if(r.type == registrar::type_enum::logic)
        { LOGIC_ITEM(r.name, r.id, r.maker); }
        else if(r.type == registrar::type_enum::output)
        { OUTPUT_ITEM(r.name, r.id, r.maker); }
        else if(r.type == registrar::type_enum::misc)
        {
            // We keep spots vacant instead of deleting because
            // deleting anywhere but the end of a vector triggers
            // a relocation.
            impl::misc_subsystem_keeper* instance = nullptr;
            impl::misc_subsystem_keeper* vacancy  = nullptr;
            for(auto& misc_sub : i.misc_subsystems)
            {
                if(misc_sub.id == r.id)                         { instance = &misc_sub; }
                else if(misc_sub.id == registrar::invalid_id()) { vacancy  = &misc_sub; }
            }

            auto const is_instantiated = instance != nullptr;
            auto const has_vacancy     = vacancy  != nullptr;
            if( ImGui::MenuItem(r.name, nullptr, is_instantiated) )
            {
                auto newval = !is_instantiated
                    ? cvt::rc<registrar::misc_subsystem_maker>(r.maker)()
                    : nullptr;

                if(!is_instantiated && has_vacancy)
                {
                    vacancy->id = r.id;
                    vacancy->instance.reset(newval);
                }
                else if(!is_instantiated && !has_vacancy)
                { i.misc_subsystems.push_back({r.id, cvt::toe * newval}); }
                else if(is_instantiated)
                {
                    instance->id = registrar::invalid_id();
                    instance->instance.reset(newval);
                }
            }
        }

        ImGui::EndMenu();
    }

    ImGui::EndMenu();

    #undef OUTPUT_ITEM
    #undef LOGIC_ITEM
    #undef INPUT_ITEM
    #undef MENU_ITEM
}

auto pinguim::app::subsystems::manager::loop_misc(float delta_seconds) -> void
{ for(auto& misc : pimpl->misc_subsystems) { if(misc.instance) misc.instance->loop(delta_seconds); } }

auto pinguim::app::subsystems::manager::update_gameinfo(float delta_seconds) -> bool
{
    if(pimpl->input) return pimpl->input->update_gameinfo(pimpl->gi, delta_seconds);
    return false;
}
auto pinguim::app::subsystems::manager::run_logic(float delta_seconds) -> bool
{
    if(pimpl->logic) return pimpl->logic->run_logic(pimpl->gi, pimpl->c, delta_seconds);
    return false;
}
auto pinguim::app::subsystems::manager::transmit(float delta_seconds) -> bool
{
    if(pimpl->output) return pimpl->output->transmit(pimpl->c, delta_seconds);
    return false;
}
