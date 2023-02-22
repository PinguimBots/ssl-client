#include "pinguim/app/subsystems/logic/strategy.hpp"

#include "pinguim/app/subsystems/registrar.hpp"
#include "pinguim/vsss/command.hpp"
#include "pinguim/vsss/control.hpp"
#include "pinguim/dont_forget.hpp"
#include "pinguim/math.hpp"
#include "pinguim/list.hpp"
#include "pinguim/cvt.hpp"

#include <complex>

#include <fmt/core.h>

PINGUIM_APP_REGISTER_LOGIC_SUBSYSTEM(pinguim::app::subsystems::logic::strategy, "Strategy");

constexpr auto to_cpx    = pinguim::cvt::custom([](auto v){ return std::complex<double>{v.x, v.y}; });
constexpr auto to_fpoint = pinguim::cvt::custom([](auto v){ return pinguim::geo::fpoint{ std::real(v), std::imag(v) }; });
constexpr auto to_ball   = pinguim::cvt::custom([](auto v){ return pinguim::vsss::common::ball{ v.location*to_cpx, v.velocity*to_cpx }; });
constexpr auto to_robot  = pinguim::cvt::custom([](auto v){ return pinguim::vsss::common::robot{ v.id, v.location * to_cpx, v.rotation, v.velocity * to_cpx, v.angular_velocity }; });

namespace pinguim::app::subsystems::logic
{
    auto strategy::run_logic(
        game_info const& gi,
        commands& commands,
        [[maybe_unused]] float delta_seconds
    ) -> bool
    {
        auto& bds = gi.field_info.bounds;

        auto bounds = pinguim::vsss::field_geometry{
            { bds[10] * to_cpx, bds[11] * to_cpx,  bds[8] * to_cpx, bds[9] * to_cpx },
            { bds[2]  * to_cpx, bds[3]  * to_cpx,  bds[4] * to_cpx, bds[5] * to_cpx },
            { bds[0]  * to_cpx, bds[1]  * to_cpx,  bds[6] * to_cpx, bds[7] * to_cpx }
        };

        fmt::print("left_bounds = [ ");
        for(auto const& lp : bounds.left_goal_bounds) { fmt::print("{{{:.2f} {:.2f}}} ", lp.real(), lp.imag()); }
        fmt::print("]\nright_bounds = [ ");
        for(auto const& lp : bounds.right_goal_bounds) { fmt::print("{{{:.2f} {:.2f}}} ", lp.real(), lp.imag()); }
        fmt::print("]\nfield_bounds = [ ");
        for(auto const& lp : bounds.field_bounds) { fmt::print("{{{:.2f} {:.2f}}} ", lp.real(), lp.imag()); }
        fmt::print("]\n\n");

        strat.setBounds(pinguim::vsss::field_geometry{
            { bds[10] * to_cpx, bds[11] * to_cpx,  bds[9] * to_cpx, bds[8] * to_cpx },
            { bds[2]  * to_cpx, bds[3]  * to_cpx,  bds[4] * to_cpx, bds[5] * to_cpx },
            { bds[0]  * to_cpx, bds[1]  * to_cpx,  bds[6] * to_cpx, bds[7] * to_cpx }
        });

        auto b = gi.ball_info * to_ball;

        auto lc = list<command>::from_container( s::move(commands) );
        PINGUIM_DONT_FORGET(commands = lc.surrender());
        lc.clear();
        lc.reserve(gi.allied_team.size());

        auto obstacles = std::vector< pinguim::vsss::point >{2};
        fmt::print("[ball] {{{:.2f}, {:.2f}}}\n", gi.ball_info.location.x, gi.ball_info.location.y);
        for(auto const& _r : gi.allied_team)
        {
            for(auto const& _r2 : gi.allied_team) if(_r2.id != _r.id) obstacles.push_back(_r2.location * to_cpx);
            auto r = _r * to_robot;
            
            auto const [new_point, rotation] = strat.actions(r, b, obstacles);
            auto const [_left, _right] = pinguim::vsss::to_pair( pinguim::vsss::control::generate_vels(r, new_point, rotation) );
            auto const left  = pb::m::map(_left  * cvt::to<float>, -100.f, 100.f, -1.f, 1.f);
            auto const right = pb::m::map(_right * cvt::to<float>, -100.f, 100.f, -1.f, 1.f);
            fmt::print("[{}] pos = {{{:.2f}, {:.2f}}} lr = {{{:.2f}, {:.2f}}}, rotation = {}\n", r.id, _r.location.x, _r.location.y, left, right, rotation);
            
            lc.push_back({gi.allied_team_id, r.id * cvt::toe, left + cvt::toe, right + cvt::toe});

            obstacles.clear();
        }
        fmt::print("\n---\n\n");

        return false;
    }
}
