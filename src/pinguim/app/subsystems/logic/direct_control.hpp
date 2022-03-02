#pragma once

#include <vector>

#include "pinguim/app/subsystems/types.hpp"

namespace pinguim::app::subsystems::logic
{
    struct direct_control : public pinguim::app::logic_subsystem
    {
        auto run_logic(game_info const&, commands&, float delta_seconds) -> bool override;

    private:
        enum class input_types {
            none,
            keyboard_WASD,
            keyboard_IJKL,
            keyboard_ARROWS,
        };

        struct input_state {
            bool fwd   = false;
            bool bkw   = false;
            bool left  = false;
            bool right = false;
        };

        auto draw_inputs_window(game_info const& gi, commands&, float delta_seconds) -> void;
        // Returns the current command.
        auto draw_input_type_combo(u32 robot_id, input_types& selected) -> void;
        auto get_input_state(input_types input) -> input_state;

        std::vector<input_types> robot_input_types = {};
        // Used for lerping.
        std::vector<command> prev_inputs = {};

        bool draw_window = true;
    };
}