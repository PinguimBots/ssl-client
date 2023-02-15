#include "pinguim/app/subsystems/logic/strategy.hpp"

#include "pinguim/app/subsystems/registrar.hpp"

#include <fmt/core.h>

PINGUIM_APP_REGISTER_LOGIC_SUBSYSTEM(pinguim::app::subsystems::logic::strategy, "Strategy");

namespace pinguim::app::subsystems::logic
{
    auto strategy::run_logic(
        game_info const& gi,
        commands& commands,
        [[maybe_unused]] float delta_seconds
    ) -> bool
    {
        //strat.setBounds(pinguim::vsss::field_geometry{
        //    
        //})
        for(auto& [x, y] : gi.field_info.bounds)
        {
            //fmt::print("x = {}, y = {}\n", x, y);
        }
        return false;
    }
}
