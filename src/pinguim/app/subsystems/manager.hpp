#pragma once

#include <memory> // For smart ptrs.

#include "pinguim/app/subsystems/types.hpp"

// Forward decl. Defined in <SDL.h>
union SDL_Event;

namespace pinguim::app::subsystems
{
    struct manager : public input_subsystem, public logic_subsystem, public output_subsystem
    {
        auto draw_selector_ui() -> void;

        auto input(pinguim::app::input_subsystem*) -> void;
        auto logic(pinguim::app::logic_subsystem*) -> void;
        auto output(pinguim::app::output_subsystem*) -> void;
        template <typename T, typename... Args> auto input(Args&&... args)  -> void { input(  new T(std::forward<Args>(args)...) ); }
        template <typename T, typename... Args> auto logic(Args&&... args)  -> void { logic(  new T(std::forward<Args>(args)...) ); }
        template <typename T, typename... Args> auto output(Args&&... args) -> void { output( new T(std::forward<Args>(args)...) ); }

        // Meant to be used with pinguim::imgui::plumber.
        auto handle_event(SDL_Event& e) -> bool override;

        auto update_gameinfo(game_info&, float delta_seconds) -> bool override;
        auto run_logic(game_info const&, commands&, float delta_seconds) -> bool override;
        auto transmit(commands const&, float delta_seconds) -> bool override;

    private:
        std::unique_ptr<pinguim::app::input_subsystem> _input{ nullptr };
        std::unique_ptr<pinguim::app::logic_subsystem> _logic{ nullptr };
        std::unique_ptr<pinguim::app::output_subsystem> _output{ nullptr };

        // Just for keeping track of stuff.
        enum class input_type  {none, firasim, vision}          input_t  = input_type::none;
        enum class logic_type  {none, direct_control, strategy} logic_t  = logic_type::none;
        enum class output_type {none, firasim}                  output_t = output_type::none;
    };
}