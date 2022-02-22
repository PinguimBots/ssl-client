#include "pinguim/app/subsystems/manager.hpp"

pinguim::app::subsystems::manager::manager()
    : input{ nullptr }
    , logic{ nullptr }
{}

auto pinguim::app::subsystems::manager::handle_event(SDL_Event& e) -> void
{
    if(input && input->handle_event(e)) return;
    if(logic && logic->handle_event(e)) return;
}