#pragma once

#include <vector>
#include <tuple>
#include <math.h>

#include "pbts/common.hpp"

namespace pbts
{

    class Strategy
    {
    public:
        Strategy();
        // Generates the final position for each allied robot.
        auto generate_robot_positions(
            const pbts::field_geometry &field,
            const std::vector<pbts::robot> &allied_robots,
            const std::vector<pbts::robot> &enemy_robots,
            const pbts::ball &ball /*,
        const pbts::game_info& game_state*/
            )
            -> std::vector<pbts::point>;

    private:
        const int xT = 200; //cm
        const int yT = 150; //cm
        const int dx = 5;   //cm
        const int dy = 5;   //cm
        const int M = (int)std::round(xT / dx);
        const int N = (int)std::round(yT / dy);

        auto create_path(
            const pbts::point goal_position,
            const pbts::robot &allied_robot,
            const std::vector<pbts::robot> &enemy_robots) 
            -> pbts::point;
        auto four_neighborhood(pbts::wpoint point) -> std::vector<pbts::wpoint>;
        auto d_neighborhood(pbts::wpoint point) -> std::vector<pbts::wpoint>;
        auto valid_neighboors(pbts::wpoint point) -> std::vector<pbts::wpoint>;
        auto wave_planner(
            const pbts::wpoint goal_position,
            const pbts::wpoint allied_robot,
            const std::vector<pbts::wpoint> &enemy_robots)
            -> pbts::wpoint;
        auto generate_obstacle(int **field, const std::vector<pbts::wpoint> &enemy_robots) -> void;
        auto recursive_wave(int **field, const std::vector<pbts::wpoint> points, int prev_cost) -> void;
        auto pertoBola(std::vector<pbts::robot>& allied_robots, const std::vector<pbts::robot> &enemy_robots,  const pbts::ball& ball) -> int;
    };
} // namespace pbts
