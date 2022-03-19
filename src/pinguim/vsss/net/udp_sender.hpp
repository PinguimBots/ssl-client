#pragma once

#include "pinguim/aliases.hpp"

namespace pinguim::vsss::net
{
    namespace udp_sender_detail { struct sv { const char* data; u64 len; }; }

    struct udp_sender
    {
        constexpr udp_sender() = default;
        udp_sender(udp_sender_detail::sv ip, u16 port);
        udp_sender(udp_sender&&);
        ~udp_sender();

        auto operator=(udp_sender&&) noexcept -> udp_sender&;

        udp_sender(const udp_sender&) = delete;
        auto operator=(const udp_sender&) -> udp_sender& = delete;

        // Returns how many bytes were written.
        auto send(u8 const* data, u64 size) -> u64;

        /// TODO: implement, see https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/reference/basic_datagram_socket/async_send_to/overload1.html
        /*
        auto async_send(u8* data, u64 size);
        */

        constexpr auto is_init() const noexcept -> bool
        { return pimpl != nullptr; }

    private:
        struct impl;
        impl* pimpl = nullptr;
    };
}