#include "pinguim/app/subsystems/logic/strategy.hpp"

#include "pinguim/app/subsystems/registrar.hpp"

PINGUIM_APP_REGISTER_LOGIC_SUBSYSTEM(pinguim::app::subsystems::logic::strategy, "Strategy");

namespace pinguim::app::subsystems::logic
{
    auto strategy::run_logic(game_info const& gi, commands &, float delta_seconds) -> bool
    {
        return false;
    }
}
