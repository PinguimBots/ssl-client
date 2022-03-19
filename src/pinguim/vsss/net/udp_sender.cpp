#include "pinguim/vsss/net/udp_sender.hpp"

#include "pinguim/standalone/forward.hpp"

#include "pinguim/vsss/net/pre_asio.hpp"
#include <asio.hpp>

#include <string_view>

struct pinguim::vsss::net::udp_sender::impl
{
    impl(udp_sender_detail::sv ip, u16 port)
        : io{}
        , strand{ asio::make_strand(io) }
        , endpoint{ asio::ip::make_address(std::string_view{ip.data, ip.len}), port }
        , socket{ strand, asio::ip::udp::v4() }
    {}

    ~impl() { io.stop(); }

    asio::io_service io;
    // We use a strand to serialize access to the socket.
    asio::strand<asio::io_service::executor_type> strand;

    asio::ip::udp::endpoint endpoint;
    asio::ip::udp::socket socket;
};

pinguim::vsss::net::udp_sender::udp_sender(udp_sender_detail::sv ip, u16 port)
    : pimpl{ new impl(ip, port) }
{}

pinguim::vsss::net::udp_sender::udp_sender(udp_sender&& other)
{ *this = std::move(other); }

pinguim::vsss::net::udp_sender::~udp_sender()
{ if(pimpl != nullptr) delete pimpl; }

auto pinguim::vsss::net::udp_sender::operator=(udp_sender&& other) noexcept -> udp_sender&
{
    auto temp   = pimpl;
    pimpl       = other.pimpl;
    other.pimpl = temp;
    return *this;
}

auto pinguim::vsss::net::udp_sender::send(u8 const* data, u64 size) -> u64
{
    return pimpl->socket.send_to(
        asio::const_buffer{ data, size },
        pimpl->endpoint
    );
}
