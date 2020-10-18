#pragma once

#include <string_view>
#include <functional>  // std::function.
#include <cstdint>     // std::uint16_t.

#include <QtNetwork>

#include "pb/packet.pb.h"

namespace pbts {

    class simulator_connection
    {
    public:
        struct connection_params {
            std::string_view ip;
            std::uint16_t port;
        };
    
        explicit simulator_connection(
            connection_params in_params,
            connection_params out_params,
            std::function<void(fira_message::sim_to_ref::Environment)> on_receive
        );

        virtual ~simulator_connection() = default;

        auto send(const fira_message::sim_to_ref::Packet& packet) -> bool;
    
    private:
        QUdpSocket listen_socket;
        QUdpSocket send_socket;
        QHostAddress out_addr;
        std::uint16_t out_port;
    };

}
