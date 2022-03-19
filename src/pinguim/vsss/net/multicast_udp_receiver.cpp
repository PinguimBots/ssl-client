#include "pinguim/vsss/net/multicast_udp_receiver.hpp"

#include "pinguim/standalone/forward.hpp"
#include "pinguim/list.hpp"

#include "pinguim/vsss/net/pre_asio.hpp"
#include <asio.hpp>

#include <condition_variable>
#include <string_view>
#include <thread>
#include <atomic>

struct pinguim::vsss::net::multicast_udp_receiver::impl
{
    std::atomic<bool> has_new_data;

    list<u8> buffer;

    asio::io_service io;
    // We use a strand to serialize access to buffer.
    asio::strand<asio::io_service::executor_type> strand;
    std::jthread worker_thread;

    asio::ip::udp::endpoint listen_endpoint;
    asio::ip::address multicast_address;
    asio::ip::udp::socket socket;

    auto queue_async_receive() -> void
    {
        socket.async_receive(
            asio::buffer(asio::mutable_buffer{ buffer.begin(), buffer.capacity() }),
            [this]([[maybe_unused]] auto ec, auto bytes){
                buffer.override_size(bytes);
                queue_async_receive();
                has_new_data.store(true);
            }
        );
    }

    impl(const char* ip, u64 ip_len, u16 port, u64 bufsize)
        : has_new_data{ false }
        , buffer( bufsize )
        , io{}
        , strand{ asio::make_strand(io) }
        , listen_endpoint{ asio::ip::address_v4::any(), port }
        , multicast_address{ asio::ip::make_address(std::string_view{ ip, ip_len }) }
        , socket{ strand }
    {
        socket.open(listen_endpoint.protocol());
        socket.set_option(asio::ip::udp::socket::reuse_address(true));
        socket.bind(listen_endpoint);
        socket.set_option(asio::ip::multicast::join_group(multicast_address));

        queue_async_receive();

        worker_thread = std::jthread{ [&]{ io.run(); }};
    }

    ~impl() { io.stop(); }
};

pinguim::vsss::net::multicast_udp_receiver::multicast_udp_receiver(
    const char* ip,
    u64 ip_len,
    u16 port,
    u64 size
) : pimpl{ new impl(ip, ip_len, port, size) }
{}

pinguim::vsss::net::multicast_udp_receiver::multicast_udp_receiver(multicast_udp_receiver&& other)
{ *this = s::move(other); }

pinguim::vsss::net::multicast_udp_receiver::~multicast_udp_receiver()
{ if(pimpl != nullptr) delete pimpl; }

auto pinguim::vsss::net::multicast_udp_receiver::operator=(multicast_udp_receiver&& other) noexcept -> multicast_udp_receiver&
{
    auto temp = pimpl;
    pimpl = other.pimpl;
    other.pimpl = temp;
    return *this;
}

auto pinguim::vsss::net::multicast_udp_receiver::read(
    read_callback cb,
    void* userdata
) -> bool
{
    if(pimpl == nullptr) { return false; }

    if(!pimpl->has_new_data.load()) { return false; }

    // std::condition_variable is a tricky thing, read up.
    auto done_mut = std::mutex{};
    auto cv = std::condition_variable{};
    auto done = false;

    asio::dispatch(pimpl->strand, [&]{
        auto container = pinguim::container{
            pimpl->buffer.begin(),
            pimpl->buffer.size(),
            pimpl->buffer.capacity()
        };
        cb(container, userdata);

        {
            auto done_guard = std::unique_lock{done_mut};
            done = true;
        }
        cv.notify_all();

        pimpl->has_new_data.store(false);
    });
    auto done_guard = std::unique_lock{done_mut};
    cv.wait(done_guard, [&]{ return done; });

    return true;
}
