#include "pinguim/app/subsystems/logic/direct_control.hpp"

#include <SDL.h>

namespace pinguim::app::subsystems::logic
{
    auto direct_control::run_logic(game_info const& gi, commands &, float delta_seconds) -> bool
    {
        //auto keys = SDL_GetKeyState(nullptr);

        return false;
    }
}
