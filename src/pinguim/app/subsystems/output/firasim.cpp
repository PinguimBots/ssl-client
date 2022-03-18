#include "pinguim/app/subsystems/output/firasim.hpp"

#include "pinguim/app/subsystems/registrar.hpp"
#include "pinguim/app/cmdline.hpp"
#include "pinguim/dont_forget.hpp"
#include "pinguim/cvt.hpp"

#include <imgui.h>

PINGUIM_APP_REGISTER_OUTPUT_SUBSYSTEM(pinguim::app::subsystems::output::firasim, "Firasim");

namespace pinguim::app::subsystems::output
{
    firasim::firasim(std::string_view _addr, u16 _port)
        : addr{}
        , addr_str_len{ _addr.size() * cvt::toe }
        , port{ _port }
        , sender{}
        , packet{}
    {
        for(auto i=0u; i < addr_str_len && i < addr_str_size; ++i) addr[i] = _addr[i];
        sender = sender_t{{addr, addr_str_len}, port};
    }

    auto firasim::transmit(commands const& c, [[maybe_unused]] float delta_seconds) -> bool
    {
        ImGui::Begin("[OUTPUT] Firasim");
        if(ImGui::InputText(
            "Addr",
            addr,
            addr_str_size,
            ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue
        ))
        {
            // strlen.
            for(addr_str_len = 0; addr[addr_str_len] != '\0'; ++addr_str_len);

            auto const is_valid_address = cmdline::parse::ipv4({addr, addr_str_len}).has_value();
            if(is_valid_address)
            { sender = sender_t{{addr, addr_str_len}, port}; }
            else
            { sender = sender_t{}; }
        }
        ImGui::End();

        if(!sender.is_init()) { return false; }

        packet.Clear();
        for(auto const& cmd : c)
        {
            auto newcmd = packet.mutable_cmd()->add_robot_commands();
            newcmd->set_id(cmd.robot_id);
            newcmd->set_yellowteam(cmd.team_id == 0);
            newcmd->set_wheel_left(static_cast<i16>(cmd.left_motor * 255));
            newcmd->set_wheel_right(static_cast<i16>(cmd.right_motor * 255));
        };

        auto const bufsize = packet.ByteSizeLong();
        auto buf = new u8[bufsize];
        PINGUIM_DONT_FORGET( delete[] buf );

        packet.SerializeToArray(buf, cvt::toe * bufsize);

        return sender.send(buf, bufsize) > 0;
    }
}