#pragma once

#include <memory> // For smart ptrs.

#include "pinguim/app/subsystem_types.hpp"

// Forward decl. Defined in <SDL.h>
union SDL_Event;

namespace pinguim::app::subsystems
{
    struct manager
    {
        manager();

        // Meant to be used with pinguim::imgui::plumber.
        auto handle_event(SDL_Event& e) -> void;

        std::unique_ptr<pinguim::app::input_subsystem> input;
        std::unique_ptr<pinguim::app::logic_subsystem> logic;
    };
}