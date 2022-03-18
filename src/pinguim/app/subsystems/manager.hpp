#pragma once

namespace pinguim::app::subsystems
{
    struct manager
    {
        manager();
        ~manager();

        auto draw_selector_ui(float delta_seconds) -> void;
        auto loop_misc(float delta_seconds)       -> void;
        auto update_gameinfo(float delta_seconds) -> bool;
        auto run_logic(float delta_seconds) -> bool;
        auto transmit(float delta_seconds) -> bool;

    private:
        struct impl;
        impl* pimpl;
    };
}