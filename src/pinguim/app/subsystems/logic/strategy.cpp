#include "pinguim/app/subsystems/logic/strategy.hpp"

#include "pinguim/app/subsystems/registrar.hpp"

PINGUIM_APP_REGISTER_LOGIC_SUBSYSTEM(pinguim::app::subsystems::logic::strategy, "Strategy");

namespace pinguim::app::subsystems::logic
{
    auto strategy::run_logic(
        [[maybe_unused]] game_info const& gi,
        [[maybe_unused]]commands &,
        [[maybe_unused]] float delta_seconds
    ) -> bool
    {
        return false;
    }
}
