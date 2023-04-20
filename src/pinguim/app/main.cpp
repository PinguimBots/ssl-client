#include <csignal>
#include <cstdlib> // For std::exit;
#include <cstdio>

#include "pinguim/app/subsystems/manager.hpp"
#include "pinguim/imgui/fonts/loader.hpp"
#include "pinguim/imgui/plumber.hpp"
#include "pinguim/dont_forget.hpp"
#include "pinguim/chrono.hpp"

#include "pinguim/app/subsystems/misc/profiler.hpp"

auto quit_signal = false;

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    std::signal(SIGINT, []([[maybe_unused]] int signal) {
        std::printf("SIGINT Received, Closing\n");
        quit_signal = true;
    });
    std::signal(SIGABRT, []([[maybe_unused]] int signal) {
        std::printf("SIGABRT Received. Something went bad!\n");
        std::exit(1);
    });

    auto mario = pinguim::imgui::make_plumber();
    if(mario == nullptr)
    {
        std::printf("Failed making mario, exiting!\n");
        return 1;
    }
    PINGUIM_DONT_FORGET( delete mario );

    auto& sm = pinguim::app::subsystems::manager::instance();
    auto dt = 0.f;

    auto quit = false;
    while(!quit && !quit_signal){
        PINGUIM_PROFILE("main");
        dt = pinguim::time([&]{
            {
                PINGUIM_PROFILE("main::logic");
                quit = mario->handle_event();

                auto const should_rebuild_fonts = pinguim::imgui::fonts::need_rebuild();
                mario->begin_frame(should_rebuild_fonts);
                pinguim::imgui::fonts::notify_rebuild_status(should_rebuild_fonts);

                sm.draw_selector_ui(dt);
                sm.loop_misc(dt);
                sm.update_gameinfo(dt);
                sm.run_logic(dt);
                sm.transmit(dt);
            }
            {
                PINGUIM_PROFILE("main::draw_frame");
                mario->draw_frame();
            }
        });
    }

    return 0;
}
