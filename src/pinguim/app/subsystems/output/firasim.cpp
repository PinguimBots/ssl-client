#include "pinguim/app/subsystems/output/firasim.hpp"

#include "pinguim/vsss/net/encoders/sim.hpp" // Defines sender_t.send<packet_t>().

#include <iostream>

namespace pinguim::app::subsystems::output
{
    firasim::firasim(std::string_view addr, u16 port)
        : sender{addr, port}
        , packet{}
    {}

    auto firasim::transmit(commands const& c, [[maybe_unused]] float delta_seconds) -> bool
    {
        packet.Clear();
        for(auto const& cmd : c)
        {
            auto newcmd = packet.mutable_cmd()->add_robot_commands();
            newcmd->set_id(cmd.robot_id);
            newcmd->set_yellowteam(cmd.team_id == 0);
            newcmd->set_wheel_left(static_cast<i16>(cmd.left_motor * 255));
            newcmd->set_wheel_right(static_cast<i16>(cmd.right_motor * 255));
        };
        return sender.send(packet) > 0;
    }
}