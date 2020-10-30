#pragma once

#include <vector>

#include "pbts/common.hpp"

namespace pbts::strategy {
    // Generates the final position for each allied robot.
    auto generate_robot_positions(
        const pbts::field_geometry& field,
        const std::vector<pbts::robot>& allied_robots,
        const std::vector<pbts::robot>& enemy_robots,
        const pbts::ball& ball /*,
        const pbts::game_info& game_state*/)
        -> std::vector< pbts::point >;

    auto create_path() -> pbts::point;

    //int pertoBola(const fira_message::Robot &ally_robots, const fira_message::Robot &enemy_robots, const fira_message::Ball &ball);
}
