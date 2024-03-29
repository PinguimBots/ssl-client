#include "pinguim/app/subsystems/output/serial.hpp"

#include "pinguim/app/subsystems/registrar.hpp"
#include "pinguim/dont_forget.hpp"
#include "pinguim/math.hpp"
#include "pinguim/cvt.hpp"

#include <imgui.h>
#include <fmt/core.h>

PINGUIM_APP_REGISTER_OUTPUT_SUBSYSTEM(pinguim::app::subsystems::output::serial, "Serial");

namespace pinguim::app::subsystems::output
{
    serial::serial() : port{io} {}

    auto serial::transmit(commands const& c, [[maybe_unused]] float delta_seconds) -> bool
    {
        ImGui::Begin("[OUTPUT] Serial");
        PINGUIM_DONT_FORGET( ImGui::End() );

        if(ImGui::InputText(
            "Serial Port Name",
            port_name,
            sizeof(port_name),
            ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue
        ))
        {
            try{
                port = asio::serial_port{io, port_name};
                port.set_option(asio::serial_port_base::baud_rate(baud));
            }
            catch(asio::system_error& e)
            { 
                fmt::print("Error opening serial port! {}\n", e.what());
            }
        }

        if(!port.is_open()) { return false; }

        for(auto const& cmd : c)
        {
            i16 buf[2] = {
                pb::m::map(cmd.left_motor,  -1.f, 1.f, -255.f, 255.f) * cvt::to<i16>,
                pb::m::map(cmd.right_motor, -1.f, 1.f, -255.f, 255.f) * cvt::to<i16>,
                //cmd.robot_id
            };
            asio::write( port, asio::buffer(buf, sizeof(buf) * sizeof(i16)) );
        }

        return true;
    }
}