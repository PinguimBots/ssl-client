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
        firasim(std::string_view _addr="127.0.0.1", u16 _port=20011);

        auto transmit(commands const&, float delta_seconds) -> bool override;

        using sender_t = pinguim::vsss::net::udp_sender;
        using packet_t = fira_message::sim_to_ref::Packet;

    private:
        static constexpr auto addr_str_size = 16;

        char addr[addr_str_size];
        u16 addr_str_len;
        u16 port;

        sender_t sender;
        packet_t packet;
    };
}