#pragma once

#include "pinguim/vsss/net/multicast_udp_receiver.hpp"
#include "pinguim/app/subsystems/types.hpp"
#include "pinguim/aliases.hpp"

#include <pinguim/simproto/all>

#include <string_view>

namespace pinguim::app::subsystems::input
{
    struct firasim : public pinguim::app::input_subsystem
    {
        firasim(std::string_view addr="224.0.0.1", u16 port=10002);

        auto update_gameinfo(game_info&, float delta_seconds) -> bool override;

        using receiver_t = pinguim::vsss::net::multicast_udp_receiver;
        using env_packet_t = fira_message::sim_to_ref::Environment;
        using referee_packet_t = VSSRef::ref_to_team::VSSRef_Command;

    private:
        enum class team {yellow, blue} allied_team_color;

        receiver_t env_receiver;
        env_packet_t env_packet;

        //receiver_t referee_receiver;
        referee_packet_t referee_packet;
    };
}