#include "pinguim/app/subsystems/input/vision.hpp"

#include <imgui.h>

namespace pinguim::app::subsystems::input
{
    auto vision::update_gameinfo(game_info& gi, float delta_seconds) -> bool
    {

        ImGui::Text("eu existo");
        return false;
    }
}
