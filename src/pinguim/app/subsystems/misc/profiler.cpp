#include "pinguim/app/subsystems/misc/profiler.hpp"

#include "pinguim/app/subsystems/registrar.hpp"
#include "pinguim/app/subsystems/manager.hpp"

#include <algorithm> // For std::sort.

#include <imgui.h>

#include <fmt/core.h>

PINGUIM_APP_REGISTER_MISC_SUBSYSTEM(pinguim::app::subsystems::misc::profiler, "Profiler");

auto pinguim::app::subsystems::misc::profiler::loop([[maybe_unused]] float delta_seconds) -> void
{
    std::sort(current_times.begin(), current_times.end(), [](auto a, auto b){
        if(a.begin == b.begin) return a.duration > b.duration;
        else                   return a.begin    > b.begin;
    });
    for(auto const& [name, begin, duration] : current_times) fmt::print("({:.6f}) {} --> {:.2f}ms\n", begin, name, duration * 1000);
    current_times.clear();
}

auto pinguim::app::subsystems::misc::profiler::submit_time(frame f) -> bool
{
    // registrar_id comes from the macro at the top of the file.
    auto instance = reinterpret_cast<profiler*>(manager::instance().get_subsystem_instance(registrar_id));

    if(!instance) return false;

    instance->current_times.push_back(f);
    return true;
}
