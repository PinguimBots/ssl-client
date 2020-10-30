#pragma once

#include <optional>
#include <tuple>

#include "pb/command.pb.h"
#include "pb/common.pb.h"
#include "pb/packet.pb.h"
#include "pb/replacement.pb.h"

namespace pbts {

    class Strategy
    {
        enum class goal_bound_type {
            left,
            right
        };
        

    public:
        Strategy();
        void checkGameState(const fira_message::Robot &ally_robots, const fira_message::Robot &enemy_robots, const fira_message::Ball &ball);

        //auto tick(auto allied_team, goal_bound_type allied_target, auto enemy_team);

        auto bounds_set() -> bool;
        auto set_bounds(pbts::field_geometry) -> void;
        auto createPath() -> std::vector<std::tuple<double,double,double>>;

    private:
        std::optional<pbts::field_geometry> bounds;
        //int pbts::Strategy::pertoBola(const fira_message::Robot &ally_robots, const fira_message::Robot &enemy_robots, const fira_message::Ball &ball);
    };

}
