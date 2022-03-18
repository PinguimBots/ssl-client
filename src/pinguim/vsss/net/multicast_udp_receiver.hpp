#pragma once

/// TODO: simplify.

#include <condition_variable>
#include <string_view>
#include <type_traits> // For std::remove_cvref_t.
#include <thread>
#include <atomic>
#include <array>

#include "pinguim/vsss/net/pre_asio.hpp"
#include <asio.hpp>

#include "pinguim/utils.hpp"

// TODO: pimpl me up!
namespace pinguim::vsss::net
{
    // Customization point.
    template<typename Target>
    struct decoder;
    // Default decoder (passthrough), used if no type is provided.
    template< std::size_t BufferSize >
    struct decoder< std::array<std::byte, BufferSize> >
    {
        using decoded_t = std::array<std::byte, BufferSize>;

        constexpr auto decode(decoded_t& buf) -> decoded_t& { return buf; }
    };

    constexpr auto default_receiver_buffer_size = 512;

    template<std::size_t BufferSize = default_receiver_buffer_size>
    class multicast_udp_receiver
    {
    public:
        static constexpr auto buffer_size = BufferSize;
        using buffer_t = std::array<std::byte, buffer_size>;

        multicast_udp_receiver(std::string_view ip, u16 port)
            : _has_new_data{ false }
            , buffer{}
            , io{}
            , strand{ asio::make_strand(io) }
            , listen_endpoint{ asio::ip::address_v4::any(), port }
            , multicast_address{ asio::ip::make_address(ip) }
            , socket{ strand }
        {
            socket.open(listen_endpoint.protocol());
            socket.set_option(
                asio::ip::udp::socket::reuse_address(true)
            );
            socket.bind(listen_endpoint);
            socket.set_option(
                asio::ip::multicast::join_group(multicast_address)
            );

            queue_async_receive();

            worker_thread = std::jthread{ [&]{ io.run(); }};
        }

        template<typename ExpectedType>
        auto sync(ExpectedType&& v) {
            return syncf<ExpectedType>([](auto){}, std::forward<ExpectedType>(v));
        }

        // Read into the last received data.
        // Returns whether or not there was new data since last sync.
        // Accessing the buffer is only safe inside Functor, otherwise
        // race conditions may occur.
        template <
            typename ExpectedType = buffer_t,
            typename Functor,
            typename... DecoderForwardedArgs
        >
        auto syncf(Functor&& f, DecoderForwardedArgs&&... fargs) -> bool
        {
            using decoded_t = std::remove_cvref_t< ExpectedType >;

            if(!has_new_data()) { return false; }

            auto done_mut = std::mutex{};
            auto cv = std::condition_variable{};
            auto done = false;

            asio::dispatch(strand, [&]{
                f(
                    decoder<decoded_t>{}.decode(
                        buffer,
                        std::forward<DecoderForwardedArgs>(fargs)...
                    )
                );

                {
                    auto done_guard = std::unique_lock{done_mut};
                    done = true;
                }
                cv.notify_all();

                _has_new_data.store(false);
            });
            auto done_guard = std::unique_lock{done_mut};
            cv.wait(done_guard, [&]{return done;});

            return true;
        }

        auto has_new_data() { return _has_new_data.load(); }

        virtual ~multicast_udp_receiver() { io.stop(); }

    protected:
        virtual auto queue_async_receive() -> void
        {
            socket.async_receive(
                asio::buffer( buffer ),
                [this](auto ec, auto bytes){
                    this->on_recv(ec, bytes);
                    _has_new_data.store(true);
                }
            );
        }

        // On receive we just queue another read, but subclasses may override if needed.
        virtual auto on_recv(
            [[maybe_unused]] std::error_code error_code,
            [[maybe_unused]] std::size_t bytes_received
        ) -> void { queue_async_receive(); }

        std::atomic<bool> _has_new_data;

        buffer_t buffer;

        asio::io_service io;
        // We use a strand to serialize access to buffer.
        asio::strand<asio::io_service::executor_type> strand;
        std::jthread worker_thread;

        asio::ip::udp::endpoint listen_endpoint;
        asio::ip::address multicast_address;
        asio::ip::udp::socket socket;
    };
}