// Provides structured binding support for firasim protobuf types.
#pragma once

#include "pinguim/utils.hpp"
#include "pinguim/conf.hpp"
#include "pinguim/cvt.hpp"

#include <array>

#include "simproto/common.pb.h"
#include "simproto/vssref_command.pb.h"

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
PINGUIM_UTILS_TUPLIFY(fira_message::Field, 4);

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
PINGUIM_UTILS_TUPLIFY(fira_message::Ball, 6);

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
PINGUIM_UTILS_TUPLIFY(fira_message::Robot, 7);

namespace fira_message {
    template <std::size_t I>
    auto get(const Frame& f) {
        if constexpr(I == 0 || I == 1) {
            constexpr const auto robot_getter = [](auto frame, auto i) {
                if constexpr(I == 0) return frame.robots_blue(pinguim::cvt::to<int> * i);
                else return frame.robots_yellow(pinguim::cvt::to<int> * i);
            };

            if constexpr( pinguim::conf::team_size_fixed && pinguim::conf::fixed_team_size > 0 )
            {
                auto robots = std::array<Robot, pinguim::conf::fixed_team_size>{};

                for(std::size_t i = 0; i < robots.size(); ++i) {
                    robots[i] = robot_getter(f, i);
                }
                return robots;
            }
            else
            {
                const auto robot_count = [&]{
                    if constexpr(I == 0) return f.robots_blue_size();
                    else return f.robots_yellow_size();
                }();

                auto robot_vec = std::vector<Robot>{};
                robot_vec.reserve(3 /* most likely */);
                for(auto i = 0; i < robot_count; ++i) {
                    robot_vec.push_back(robot_getter(f, i));
                }
                return robot_vec;
            }
        }
        else if constexpr(I == 2) {
            return f.ball();
        }
    }
}
PINGUIM_UTILS_TUPLIFY(fira_message::Frame, 3);

namespace VSSRef::ref_to_team {
    template <std::size_t I>
    auto get(const VSSRef_Command& c) {
        if constexpr(I == 0) {return c.foul();}
        else if constexpr(I == 1) {return c.teamcolor();}
        else if constexpr(I == 2) {return c.foulquadrant();}
        else if constexpr(I == 3) {return c.timestamp();}
        else if constexpr(I == 4) {return c.gamehalf();}
    }
}
PINGUIM_UTILS_TUPLIFY(VSSRef::ref_to_team::VSSRef_Command, 5);
