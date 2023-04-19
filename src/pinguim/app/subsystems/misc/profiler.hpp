#pragma once

#include "pinguim/app/subsystems/types.hpp"

#include <source_location>
#include <vector>
#include <string>
#include <chrono>

#define _PINGUIM_PROFILER_CONCAT(x,y) x ## y
#define PINGUIM_PROFILER_CONCAT(x,y) _PINGUIM_PROFILER_CONCAT(x,y)
#define PINGUIM_PROFILE_ME \
    auto const PINGUIM_PROFILER_CONCAT( _pinguim_profiler, __LINE__ ) \
        = pinguim::app::subsystems::misc::profiler::request_profiling()
#define PINGUIM_PROFILE( me ) \
    auto const PINGUIM_PROFILER_CONCAT( _pinguim_profiler, __LINE__ ) \
        = pinguim::app::subsystems::misc::profiler::request_profiling(me)

namespace pinguim::app::subsystems::misc
{
    struct profiler : public misc_subsystem
    {
        auto loop(float delta_seconds) -> void override;

        struct frame { std::string name; float begin, duration; };
        static auto submit_time(frame f) -> bool;

        template <typename Timer = std::chrono::high_resolution_clock>
        struct request_profiling
        {
            std::string frame_name;
            decltype(Timer::now()) t;

            request_profiling(auto s)
                : frame_name{s}
                , t{ Timer::now() }
            {}
            request_profiling()
                : frame_name{ std::source_location::current().function_name() }
                , t{ Timer::now() }
            {}
            ~request_profiling()
            {
                submit_time({
                    frame_name,
                    static_cast<float>(std::chrono::duration_cast<  std::chrono::microseconds >(t.time_since_epoch()).count()),
                    std::chrono::duration_cast< std::chrono::duration<float> >(Timer::now() - t).count()
                });
            }
        };

    private:
        std::vector< frame > current_times;
    };
}
