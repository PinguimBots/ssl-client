#include "pinguim/app/subsystems/input/vision.hpp"

#include <imgui.h>

namespace pinguim::app::subsystems::input
{
    auto vision::update_gameinfo([[maybe_unused]] game_info& gi, [[maybe_unused]] float delta_seconds) -> bool
    {
        ImGui::Text("VISION MODULE DISABLED, THE OPENCV DEPENDENCY WAS NOT FOUND");
        return false;
    }
}
