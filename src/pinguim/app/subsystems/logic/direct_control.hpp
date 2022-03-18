#pragma once

#include <vector>

#include "pinguim/app/subsystems/types.hpp"

namespace pinguim::app::subsystems::logic
{
    struct direct_control : public pinguim::app::logic_subsystem
    {
        auto run_logic(game_info const&, commands&, float delta_seconds) -> bool override;

    private:
        // _direct here means that when you press W you can control the
        // motors with A and D individually, that is, W puts the robot
        // in drive - and S in reverse - and you can control each motor.
        // as you wish.
        //
        // The non _direct control modes drive more like in video games,
        // where W means 'Go forward' - and S means 'Go backwards' - and
        // A and D mean 'If going forwards/backwards, steer left/right'.
        enum class input_types {
            none = 1,
            keyboard_WASD = 2,
            keyboard_WASD_direct = 3,
            keyboard_IJKL = 4,
            keyboard_IJKL_direct = 5,
            keyboard_ARROWS = 6,
            keyboard_ARROWS_direct = 7,
        };
        static constexpr auto is_direct(input_types i) { return static_cast<int>(i) % 2 == 1; }

        struct input_state {
            bool fwd   = false;
            bool bkw   = false;
            bool left  = false;
            bool right = false;
        };

        auto draw_inputs_window(game_info const& gi, list<command>&, float delta_seconds) -> void;
        // Returns the current command.
        auto draw_input_type_combo(u32 robot_id, input_types& selected, float& max_robot_lerp_time) -> void;
        auto get_input_state(input_types input) -> input_state;

        std::vector<input_types> robot_input_types = {};
        // Used for lerping.
        std::vector<command> prev_inputs = {};
        std::vector<float> max_lerp_time = {};
    };
}