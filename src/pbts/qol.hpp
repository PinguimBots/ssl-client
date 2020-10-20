/// General quality of life stuff.
#pragma once

#include <array>

#include "pb/common.pb.h"

/// Structured binding support for protobuf types.
// Macro for automating boilerplate.
#define PBTS_QOL_TUPLIFY(classname, tuplesize)                      \
    template <>                                                     \
    struct std::tuple_size< classname >                             \
    {                                                               \
        static const std::size_t value = tuplesize;                 \
    };                                                              \
    template <std::size_t I>                                        \
    struct std::tuple_element< I, classname >                       \
    {                                                               \
        using type = decltype(get<I>(std::declval< classname >())); \
    }

// NOTE: get() must be in the same namespace as it's respective class.

namespace fira_message {
    template <std::size_t I>
    auto get(const Field& f)
    {
        if constexpr(I == 0) {return f.length();}
        else if constexpr(I == 1) {return f.width();}
        else if constexpr(I == 2) {return f.goal_width();}
        else if constexpr(I == 3) {return f.goal_depth();}
    }
}
PBTS_QOL_TUPLIFY(fira_message::Field, 4);

namespace fira_message {
    template <std::size_t I>
    auto get(const Ball& b)
    {
        if constexpr(I == 0) {return b.x();}
        else if constexpr(I == 1) {return b.y();}
        else if constexpr(I == 2) {return b.z();}
        else if constexpr(I == 3) {return b.vx();}
        else if constexpr(I == 4) {return b.vy();}
        else if constexpr(I == 5) {return b.vz();}
    }
}
PBTS_QOL_TUPLIFY(fira_message::Ball, 6);

namespace fira_message {
    template <std::size_t I>
    auto get(const Robot& r)
    {
        if constexpr(I == 0) {return r.robot_id();}
        else if constexpr(I == 1) {return r.x();}
        else if constexpr(I == 2) {return r.y();}
        else if constexpr(I == 3) {return r.orientation();}
        else if constexpr(I == 4) {return r.vx();}
        else if constexpr(I == 5) {return r.vy();}
        else if constexpr(I == 6) {return r.vorientation();}
    }
}
PBTS_QOL_TUPLIFY(fira_message::Robot, 7);

namespace fira_message {
    template <std::size_t I>
    auto get(const Frame& f) {
        if constexpr(I == 0 || I == 1) {
            constexpr const auto robot_getter = [](auto f, auto i) {
                if constexpr(I == 0) return f.robots_blue(i);
                else return f.robots_yellow(i);
            };

            #ifdef PBTS_FIXED_TEAM_SIZE_OF

                auto robots = std::array<Robot, 
                    PBTS_FIXED_TEAM_SIZE_OF
                >{};

                for(std::size_t i = 0; i < robots.size(); ++i) {
                    robots[i] = robot_getter(f, i);
                }
                return robots;

            #else

                const auto robot_count = [&]{
                    if constexpr(I == 0) 
                        return f.robots_blue_size();
                    else return f.robots_yellow_size();
                }();

                auto robot_vec = std::vector<Robot>{};
                robot_vec.reserve(3 /* most likely */);
                for(auto i = 0; i < robot_count; ++i) {
                    robot_vec.push_back(robot_getter(f, i));
                }
                return robot_vec;
    
            #endif
        }
        else if constexpr(I == 2) {
            return f.ball();
        }
    }
}
PBTS_QOL_TUPLIFY(fira_message::Frame, 3);

#undef PBTS_QOL_TUPLIFY

/// General utility
namespace pbts {
    // When you want to declare a std::array of some type but
    // infer the size.
    template <typename V, typename... T>
    constexpr auto array_of(T&&... t)
        -> std::array < V, sizeof...(T) >
    {
        return { std::forward<T>(t)... };
    }
}
