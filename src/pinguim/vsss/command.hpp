#pragma once

#include "pinguim/aliases.hpp"

namespace pinguim::vsss
{
    struct command
    {
        u8 team_id;
        u8 robot_id;
        // These should range between:
        // -1 (Backwards at full speed);
        // 0 (Stopped);
        // and 1 (Forwards at full speed).
        float left_motor;
        float right_motor;
    };
}
