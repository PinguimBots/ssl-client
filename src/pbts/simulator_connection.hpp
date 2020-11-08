#pragma once

#include <string_view>
#include <functional>  // std::function.
#include <cstdint>     // std::uint16_t.

#include <QtNetwork>

#include "pb/packet.pb.h"
#include "pb/vssref_command.pb.h"
#include "pb/vssref_placement.pb.h"

namespace pbts {

    class simulator_connection
    {
    public:
        struct connection_params
        {
            std::string_view ip;
            std::uint16_t port;
        };
    
        explicit simulator_connection(
            connection_params simulator_in_params,
            connection_params simulator_out_params,
            std::function<void(fira_message::sim_to_ref::Environment)> on_simulator_receive,
            connection_params referee_in_params,
            connection_params replacer_out_params,
            std::function<void(VSSRef::ref_to_team::VSSRef_Command)> on_referee_receive
        );

        virtual ~simulator_connection() = default;

        auto simulator_send(const fira_message::sim_to_ref::Packet& packet) -> bool;
        auto replacer_send(const VSSRef::team_to_ref::VSSRef_Placement& placement) -> bool;
    
    private:
        QUdpSocket simulator_out_socket;
        QHostAddress simulator_out_addr;
        std::uint16_t simulator_out_port;

        QUdpSocket simulator_in_socket;

        QUdpSocket replacer_out_socket;
        QHostAddress replacer_out_addr;
        std::uint16_t replacer_out_port;

        QUdpSocket referee_in_socket;
    };

}
