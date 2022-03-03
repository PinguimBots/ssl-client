#pragma once

#include "pinguim/app/subsystems/types.hpp"
#include "pinguim/vsss/net/udp_sender.hpp"
#include "pinguim/aliases.hpp"

#include <string_view>

#include <pinguim/simproto/all>

namespace pinguim::app::subsystems::output
{
    struct firasim : public pinguim::app::output_subsystem
    {
        firasim(std::string_view addr="127.0.0.1", u16 port=20011);

        auto transmit(commands const&, float delta_seconds) -> bool override;

        using sender_t = pinguim::vsss::net::udp_sender;
        using packet_t = fira_message::sim_to_ref::Packet;

    private:
        sender_t sender;
        packet_t packet;
    };
}