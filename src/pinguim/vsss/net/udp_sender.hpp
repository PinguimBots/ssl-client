#pragma once

#include <string_view>
#include <type_traits> // For std::remove_cvref_t.
#include <cstddef> // For std::byte.
#include <span>

#include <asio.hpp>

#include "pinguim/utils.hpp"

namespace pinguim::vsss::net
{
    // Customization point.
    template<typename Target>
    struct encoded;

    // Default encoder (passthrough). Expected interface.
    template<std::size_t Size>
    struct encoded< std::array<std::byte, Size> >
    {
        using obj_t = std::array<std::byte, Size>;

        encoded(obj_t&& _obj)      : obj{ std::move(_obj) } {}
        encoded(const obj_t& _obj) : obj{ _obj }            {}

        auto begin() const { return obj.cbegin(); }
        auto size()  const { return obj.size(); }

        const obj_t obj;
    };

    class udp_sender
    {
    public:
        inline udp_sender(std::string_view ip, u16 port)
            : io{}
            , strand{ asio::make_strand(io) }
            , endpoint{ asio::ip::make_address(ip), port }
            , socket{ strand, asio::ip::udp::v4() }
        {}

        virtual ~udp_sender() { io.stop(); }

        // Returns how many bytes were written.
        template<typename T>
        inline auto send(T&& t)
        {
            using encoded_t = std::remove_cvref_t<T>;

            auto e = encoded< encoded_t >{ std::forward<T>(t) };
            return socket.send_to(
                asio::const_buffer{ e.begin(), e.size() },
                endpoint
            );
        }

        /// TODO: implement, see https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/reference/basic_datagram_socket/async_send_to/overload1.html
        /*
        template<typename T>
        auto async_send(T&& t);
        */

    protected:
        asio::io_service io;
        // We use a strand to serialize access to the socket.
        asio::strand<asio::io_service::executor_type> strand;

        asio::ip::udp::endpoint endpoint;
        asio::ip::udp::socket socket;
    };
}