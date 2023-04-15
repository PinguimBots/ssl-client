#pragma once

#include "pinguim/app/subsystems/types.hpp"
#include "pinguim/aliases.hpp"

#include <asio.hpp>
#include <string>

namespace pinguim::app::subsystems::output
{
    struct serial : public pinguim::app::output_subsystem
    {
        serial();

        auto transmit(commands const&, float delta_seconds) -> bool override;

    private:
        char port_name[32] = "";
        u16 baud = 9600;

        asio::io_service io = {};
        asio::serial_port port;
    };
}