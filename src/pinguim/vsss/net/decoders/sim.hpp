// Decoders for *sim protofiles.
#pragma once

#include "pinguim/vsss/net/multicast_udp_receiver.hpp"
#include "pinguim/cvt.hpp"

#include <simproto.hpp>

template<>
struct pinguim::vsss::net::decoder<
    fira_message::sim_to_ref::Environment
>
{
    using decoded_t = fira_message::sim_to_ref::Environment;

    template<std::size_t BufferSize>
    constexpr auto decode(std::array<std::byte, BufferSize>& buf, decoded_t& packet) -> decoded_t&
    {
        packet.ParseFromArray(buf.data(), cvt::toe << buf.size());
        return packet;
    }

    template<std::size_t BufferSize>
    inline auto decode(std::array<std::byte, BufferSize>& buf) -> decoded_t
    {
        auto d = decoded_t{};
        return decode(buf, d);
    }
};

template<>
struct pinguim::vsss::net::decoder<
    VSSRef::ref_to_team::VSSRef_Command
>
{
    using decoded_t = VSSRef::ref_to_team::VSSRef_Command;

    template<std::size_t BufferSize>
    constexpr auto decode(std::array<std::byte, BufferSize>& buf, decoded_t& packet) -> decoded_t&
    {
        packet.ParseFromArray(buf.data(), cvt::toe << buf.size());
        return packet;
    }

    template<std::size_t BufferSize>
    inline auto decode(std::array<std::byte, BufferSize>& buf) -> decoded_t
    {
        auto d = decoded_t{};
        return decode(buf, d);
    }
};
