#pragma once

#include "pinguim/app/subsystems/types.hpp"

namespace pinguim::app::subsystems::misc
{
    struct imgui_debug : public misc_subsystem
    {
        auto loop(float delta_seconds) -> void override;

    private:
        bool show_metrics;
        bool show_demo;
        bool show_stack_tool;
        bool show_about;
    };
}