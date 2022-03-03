#include <csignal>
#include <cstdlib> // For std::exit;

#include <pinguim/simproto/all> // Protobuf stuff.

#include "pinguim/app/subsystems/manager.hpp"
#include "pinguim/app/cmdline.hpp"
#include "pinguim/imgui/plumber.hpp"
#include "pinguim/aliases.hpp"
#include "pinguim/chrono.hpp"

auto quit_signal = false;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::signal(SIGINT, []([[maybe_unused]] int signal) {
        fmt::print("SIGINT Received, Closing\n");
        quit_signal = true;
    });
    std::signal(SIGABRT, []([[maybe_unused]] int signal) {
        fmt::print("SIGABRT Received. Something went bad!\n");
        std::exit(1);
    });

    const auto args = pb::app::cmdline::parse_argv_or_abort(
        argc,
        const_cast<const char**>(argv)
    );

    auto sm = pb::app::subsystems::manager();

    auto dt        = 0.f;
    auto game_info = pb::app::game_info{};
    auto commands  = pb::app::commands{};

    auto mario = pb::imgui::make_plumber().value();

    auto quit = false;
    while(!quit && !quit_signal){
        dt = pb::time([&]{
            mario.handle_event( [&](auto& e){ if(!(quit = mario.quit_handler(e))) sm.handle_event(e); } );
            mario.begin_frame();

            sm.draw_selector_ui();
            sm.update_gameinfo(game_info, dt);
            sm.run_logic(game_info, commands, dt);
            sm.transmit(commands, dt);

            mario.draw_frame();
        });
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
