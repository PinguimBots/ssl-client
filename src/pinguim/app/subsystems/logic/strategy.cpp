#include "pinguim/app/subsystems/logic/strategy.hpp"

namespace pinguim::app::subsystems::logic
{
    auto strategy::run_logic(game_info const& gi, commands &, float delta_seconds) -> bool
    {
        return false;
    }
}
