#pragma once

#include <string_view>

#include "pinguim/aliases.hpp"

// Forward decl.
namespace fira_message::sim_to_ref { class Environment; }

namespace pinguim::app::ui
{
    struct windows {
        bool env_packet = true;
        bool referee_packet = false;
        bool out_packet = true;
        bool demo = false;
        bool lena = false;
        bool opencv = true;
    };

    auto draw_menubar(windows&) -> void;
    auto draw_env_packet_window(
        windows&,
        std::string_view in_address,
        u16 in_port,
        const fira_message::sim_to_ref::Environment& env_packet
    ) -> void;
    auto draw_lena_window(windows&) -> void;
    auto draw_demo_window()         -> void;
}