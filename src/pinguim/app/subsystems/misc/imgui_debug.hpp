#pragma once

#include "pinguim/app/subsystems/types.hpp"

namespace pinguim::app::subsystems::misc
{
    struct imgui_debug : public misc_subsystem
    {
        auto loop(float delta_seconds) -> void override;

    private:
        bool show_metrics = false;
        bool show_demo = false; 
        bool show_stack_tool = false;
        bool show_about = false;
    };
}