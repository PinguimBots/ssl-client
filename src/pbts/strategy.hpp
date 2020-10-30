#pragma once

#include <vector>
#include <tuple>

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
        int xT = 200; //cm
        int yT = 150; //cm
        int dx = 5;
        int dy = 5;
        int M  = xT/dx;
        int N  = yT/dy;

        auto create_path() -> pbts::point;
        auto pbts::Strategy::fourNeighborhood(int x, int y) -> std::vector<std::tuple<int,int>>;

        //int pertoBola(const fira_message::Robot &ally_robots, const fira_message::Robot &enemy_robots, const fira_message::Ball &ball);
    };
} // namespace pbts
