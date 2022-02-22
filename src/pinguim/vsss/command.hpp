#pragma once

#include "pinguim/aliases.hpp"

namespace pinguim::vsss
{
    struct command
    {
        u8 robot_id;
        u8 left_motor;
        u8 right_motor;
    };
}
