// Encoders for *sim protofiles.
#pragma once

#include "pinguim/vsss/net/udp_sender.hpp"

#include "pinguim/cvt.hpp"

#include <pinguim/simproto/all>

template<>
struct pinguim::vsss::net::encoded<
    fira_message::sim_to_ref::Packet
>
{
    using encoded_t = fira_message::sim_to_ref::Packet;

    inline encoded(const encoded_t& packet) : buf{}
    {
        buf.resize(packet.ByteSizeLong() * cvt::toe);
        /*bool success = */packet.SerializeToArray(buf.data(), buf.size() * cvt::toe);
        //if(!success) {return false;}
    }

    auto begin() const { return buf.data(); }
    auto size()  const { return buf.size(); }

    std::vector<std::byte> buf;
};
