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
        // Generates the final position for each allied robot.
        auto generate_robot_positions(
            const pbts::field_geometry &field,
            const std::vector<pbts::robot> &allied_robots,
            const std::vector<pbts::robot> &enemy_robots,
            const pbts::ball &ball /*,
        const pbts::game_info& game_state*/
            )
            -> std::vector<pbts::point>;

        static const constexpr auto xT = 2.0d; //cm
        static const constexpr auto yT = 1.5d; //cm
        static const constexpr auto dx = 0.025d;   //cm
        static const constexpr auto dy = 0.025d;   //cm

        static const constexpr auto xbias = xT/2;
        static const constexpr auto ybias = yT/2;

        static const constexpr int M = std::round(xT/dx);
        static const constexpr int N = std::round(yT/dy);

        auto create_path(
            const pbts::point goal_position,
            const pbts::robot &allied_robot,
            const std::vector<pbts::point> &enemy_robots)
            -> pbts::point;
    private:
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
        auto discreet_to_real(pbts::wpoint wpoint) -> pbts::point;
        auto real_to_discreet(pbts::point point) -> pbts::wpoint;

    };
} // namespace pbts
