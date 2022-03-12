#include <iostream>
#include <csignal>
#include <cstdlib> // For std::exit;

#include "pinguim/app/subsystems/manager.hpp"
#include "pinguim/imgui/plumber.hpp"
#include "pinguim/aliases.hpp"
#include "pinguim/chrono.hpp"

auto quit_signal = false;

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    std::signal(SIGINT, []([[maybe_unused]] int signal) {
        std::cout << "SIGINT Received, Closing" << std::endl;
        quit_signal = true;
    });
    std::signal(SIGABRT, []([[maybe_unused]] int signal) {
        std::cout << "SIGABRT Received. Something went bad!" << std::endl;
        std::exit(1);
    });

    auto sm = pb::app::subsystems::manager();

    auto dt        = 0.f;
    auto game_info = pb::app::game_info{};
    auto commands  = pb::app::commands{};

    auto mario = pb::imgui::make_plumber().value();

    auto quit = false;
    while(!quit && !quit_signal){
        dt = pb::time([&]{
            quit = mario.handle_event();
            mario.begin_frame();

            sm.draw_selector_ui(dt);
            sm.update_gameinfo(game_info, dt);
            sm.run_logic(game_info, commands, dt);
            sm.transmit(commands, dt);

            mario.draw_frame();
        });
    }

    return 0;
}
