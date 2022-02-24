#include "pinguim/app/subsystems/output/firasim.hpp"

namespace pinguim::app::subsystems::output
{
    auto firasim::transmit(commands const&, float delta_seconds) -> bool
    {
        return false;
    }
}