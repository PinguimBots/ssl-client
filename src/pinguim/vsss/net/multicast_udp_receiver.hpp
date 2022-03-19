#pragma once

#include "pinguim/container.hpp"
#include "pinguim/aliases.hpp"

namespace pinguim::vsss::net
{
    constexpr auto default_receiver_buffer_size = 512_u64;

    class multicast_udp_receiver
    {
    public:
        constexpr multicast_udp_receiver() = default;

        multicast_udp_receiver(
            const char* ip,
            u64 ip_len,
            u16 port,
            u64 size = default_receiver_buffer_size
        );
        multicast_udp_receiver(multicast_udp_receiver&&);
        ~multicast_udp_receiver();

        auto operator=(multicast_udp_receiver&&) noexcept -> multicast_udp_receiver&;

        multicast_udp_receiver(const multicast_udp_receiver&) = delete;
        auto operator=(const multicast_udp_receiver&) -> multicast_udp_receiver& = delete;

        using read_callback = void(*)(container<u8> const&, void* userdata);

        // Returns true if there was new data and the callback was called.
        auto read(read_callback, void* userdata) -> bool;

    private:
        struct impl;
        impl* pimpl = nullptr;
    };
}